#include "xcb_base.h"
#include <string.h>

XcbError::XcbError(const std::string &str, int code)
  : std::runtime_error(str + " (" + std::to_string(code) + ")"), code(code)
{
}

XcbConnectionError::XcbConnectionError(int code)
  : XcbError(std::string("XcbConnectionError: ") + get_error_string(code), code)
{
}

const char *XcbConnectionError::get_error_string(int code)
{
  static constexpr const char *errstrs[] = {
    "Success",
    "I/O error",                      // XCB_CONN_ERROR
    "Unsupported extension",          // XCB_CONN_CLOSED_EXT_NOTSUPPORTED
    "Insufficient memory",            // XCB_CONN_CLOSED_MEM_INSUFFICIENT
    "Requested length exceeded",      // XCB_CONN_CLOSED_REQ_LEN_EXCEED
    "Could not parse DISPLAY string", // XCB_CONN_CLOSED_PARSE_ERR
    "No such screen on display",      // XCB_CONN_CLOSED_INVALID_SCREEN
    "Error during FD passing"         // XCB_CONN_CLOSED_FDPASSING_FAILED
  };

  if (code >= 0 && (unsigned int)code < sizeof(errstrs) / sizeof(*errstrs)) {
    return errstrs[code];
  }
  return ""; // TODO?
}

XcbEventError::XcbEventError(int code)
  : XcbError(std::string("XcbEventError: ") + get_error_string(code), code)
{
}

XcbEventError::XcbEventError(const std::string &prefix, int code)
  : XcbError(prefix + ": " + get_error_string(code), code)
{
}

const char *XcbEventError::get_error_string(int code)
{
  static constexpr const char *errstrs[] = {
    "Success", "BadRequest", "BadValue", "BadWindow", "BadPixmap",
    "BadAtom", "BadCursor", "BadFont", "BadMatch", "BadDrawable",
    "BadAccess", "BadAlloc", "BadColor", "BadGC", "BadIDChoice",
    "BadName", "BadLength", "BadImplementation"
  };

  if (code >= 0 && (unsigned int)code < sizeof(errstrs) / sizeof(*errstrs)) {
    return errstrs[code];
  }
  return ""; // TODO?
}

XcbConnection::XcbConnection(const char *name)
{
  conn = xcb_connect(name, &default_screen_num);
  owned = true;

  const int res = xcb_connection_has_error(conn);
  if (res) {
    throw XcbConnectionError(res);
  }

  setup = xcb_get_setup(conn);
  cache_screens();
}

XcbConnection::XcbConnection(xcb_connection_t *conn, int default_screen_num)
  : conn(conn), owned(false), default_screen_num(default_screen_num)
{
  // assert(conn);   // TODO?
  // const int res = xcb_connection_has_error(conn);  // TDOO?
  setup = xcb_get_setup(conn);
  cache_screens();
}

XcbConnection::~XcbConnection()
{
  if (owned) {
    xcb_disconnect(conn);
  }
}

void XcbConnection::cache_screens()
{
  xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);

  screen_cache.clear();
  screen_cache.reserve(it.rem);
  for (; it.rem; xcb_screen_next(&it)) {
    screen_cache.push_back(it.data);
  }
}

xcb_screen_t *XcbConnection::screen_of_display(int screen_num)
{
  if (screen_num < 0 || (size_t)screen_num >= screen_cache.size()) {
    return NULL;
  }
  return screen_cache[screen_num];
}

std::pair<const xcb_visualtype_t *, uint8_t> XcbConnection::visualtype(xcb_screen_t *screen, xcb_visualid_t vid)
{
  if (!screen) {
    return {NULL, 0};
  }

  xcb_depth_iterator_t it = xcb_screen_allowed_depths_iterator(screen);
  for (; it.rem; xcb_depth_next(&it)) {
    xcb_visualtype_iterator_t jt = xcb_depth_visuals_iterator(it.data);
    for (; jt.rem; xcb_visualtype_next(&jt)) {
      if (vid == jt.data->visual_id) {
        return {jt.data, it.data->depth};
      }
    }
  }
  return {NULL, 0};
}

void XcbConnection::flush()
{
  const int res = xcb_flush(conn);
  if (res <= 0) {
    const int code = xcb_connection_has_error(conn);
    if (res) {
      throw new XcbConnectionError(code);
    }
  } // else: success
}

XcbFuture<xcb_intern_atom_request_t, detail::intern_atom_atom> XcbConnection::intern_atom(const char *name, bool create)
{
  // assert(name);
  return {conn, !create, strlen(name), name};
}

const xcb_format_t *XcbConnection::format(uint8_t depth)
{
  xcb_format_iterator_t it = xcb_setup_pixmap_formats_iterator(setup);
  for (int i = 0; it.rem; xcb_format_next(&it), i++) {
    if (it.data->depth == depth) {
      return it.data;
    }
  }
  return NULL;
}

XcbColor XcbConnection::color(uint16_t red, uint16_t green, uint16_t blue)
{
  xcb_colormap_t cmap = default_colormap(); // TODO?
  XcbFuture<xcb_alloc_color_request_t> ac{conn, cmap, red, green, blue};
  return {conn, cmap, ac.get()->pixel};
}


XcbColor::~XcbColor()
{
  xcb_free_colors(conn, cmap, ~0, 1, &pixel);  // (unchecked)
}


XcbWindow::XcbWindow(
  XcbConnection &conn, xcb_window_t parent,
  uint16_t width, uint16_t height,
  uint32_t value_mask, std::initializer_list<const uint32_t> value_list,
  uint16_t klass,
  int16_t x, int16_t y, uint16_t border_width,
  uint8_t depth, xcb_visualid_t visual)
  : conn(conn), win(conn.generate_id())
{
#if 1
  xcb_void_cookie_t ck = xcb_create_window_checked(
    conn,
    depth,
    win, parent,
    x, y, width, height, border_width,
    XCB_WINDOW_CLASS_INPUT_OUTPUT, visual,
    value_mask, value_list.begin());

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    throw XcbEventError(error->error_code);
  }
#else
  xcb_create_window_checked(
    conn,
    depth,
    win, parent,
    x, y, width, height, border_width,
    XCB_WINDOW_CLASS_INPUT_OUTPUT, visual,
    value_mask, value_list);

  // conn.flush(); ?
#endif
}

XcbWindow::~XcbWindow()
{
#if 0
  xcb_void_cookie_t ck = xcb_destroy_window_checked(conn, win);

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    fprintf(stderr, "xcb_destroy_window failed: %s (%d)\n",
      XcbEventError::get_error_string(error->error_code),
      error->error_code);
//    throw XcbEventError(error->error_code);   // dtor shall be nothrow
  }
#else
  xcb_destroy_window(conn, win);
  conn.flush();
#endif
}

std::pair<xcb_atom_t, xcb_atom_t> XcbWindow::install_delete_handler()
{
  auto wmproto = conn.intern_atom("WM_PROTOCOLS");
  auto wmdel = conn.intern_atom("WM_DELETE_WINDOW");

  xcb_atom_t wmprotocols_atom = wmproto.get();
  xcb_atom_t wmdelete_atom = wmdel.get();

  if (wmprotocols_atom == XCB_ATOM_NONE) {
    fprintf(stderr, "WM_PROTOCOLS not available\n");
    return { XCB_ATOM_NONE, XCB_ATOM_NONE };
  } else if (wmdelete_atom == XCB_ATOM_NONE) {
    fprintf(stderr, "WM_DELETE_WINDOW not available\n");
    return { XCB_ATOM_NONE, XCB_ATOM_NONE };
  }

  const xcb_atom_t props[] = {
    wmdelete_atom
  };
#if 1
  xcb_void_cookie_t ck = xcb_change_property_checked(conn, XCB_PROP_MODE_REPLACE, win, wmprotocols_atom, XCB_ATOM_ATOM, 8 * sizeof(*props), sizeof(props)/sizeof(*props), props);

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    const int code = error->error_code;
    throw XcbEventError("xcb_change_property_checked error", code);
  }
#else
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win, wmprotocols_atom, XCB_ATOM_ATOM, 8 * sizeof(*props), sizeof(props)/sizeof(*props), props);
//  conn.flush();  // ?
#endif

  return {wmprotocols_atom, wmdelete_atom};
}

void XcbWindow::map()
{
#if 1
  xcb_void_cookie_t ck = xcb_map_window_checked(conn, win);

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    const int code = error->error_code;
    throw XcbEventError(code);
  }
#else
  xcb_map_window(conn, win);
  conn.flush();  // ?
#endif
}

void XcbWindow::unmap()
{
#if 1
  xcb_void_cookie_t ck = xcb_unmap_window_checked(conn, win);

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    const int code = error->error_code;
    throw XcbEventError(code);
  }
#else
  xcb_unmap_window(conn, win);
  conn.flush();
#endif
}

void XcbWindow::change(uint32_t value_mask, std::initializer_list<const uint32_t> value_list)
{
#if 1
  xcb_void_cookie_t ck = xcb_change_window_attributes_checked(conn, win, value_mask, value_list.begin());

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    const int code = error->error_code;
    throw XcbEventError(code);
  }
#else
  xcb_change_window_attributes(conn, win, value_mask, value_list.begin());
  conn.flush();
#endif
}


XcbGC::XcbGC(
  XcbConnection &conn, xcb_drawable_t drawable,
//  uint32_t value_mask, const void *value_list,
  uint32_t value_mask, std::initializer_list<const uint32_t> value_list)
  : conn(conn), gc(conn.generate_id())
{
  xcb_void_cookie_t ck = xcb_create_gc_checked(
    conn, gc,
    drawable,
    value_mask, value_list.begin());

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    throw XcbEventError(error->error_code);
  }
}

XcbGC::~XcbGC()
{
#if 0
  xcb_void_cookie_t ck = xcb_free_gc_checked(conn, gc);

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    fprintf(stderr, "xcb_free_gc failed: %s (%d)\n",
      XcbEventError::get_error_string(error->error_code),
      error->error_code);
//    throw XcbEventError(error->error_code);   // dtor shall be nothrow
  }
#else
  xcb_free_gc(conn, gc);
  conn.flush();
#endif
}

void XcbGC::change(uint32_t value_mask, std::initializer_list<const uint32_t> value_list)
{
#if 1
  xcb_void_cookie_t ck = xcb_change_gc_checked(conn, gc, value_mask, value_list.begin());

  unique_xcb_generic_error_t error{xcb_request_check(conn, ck)};
  if (error) {
    const int code = error->error_code;
    throw XcbEventError(code);
  }
#else
  xcb_change_gc(conn, gc, value_mask, value_list.begin());
#endif
}

