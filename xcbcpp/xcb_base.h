#pragma once

#include <xcb/xcb.h>
#include <stdexcept>
#include <vector>

struct XcbError : std::runtime_error {
  XcbError(const std::string &str, int code);

  int code;
};

struct XcbConnectionError : XcbError {
  XcbConnectionError(int code);

  static const char *get_error_string(int code);
};


struct XcbEventError : XcbError {
  XcbEventError(int code);
  XcbEventError(const std::string &prefix, int code);

  static const char *get_error_string(int code);
};

#include "xcb_proto.tcc"
#include "xcb_future.h"

namespace detail {
struct intern_atom_atom {
  static xcb_atom_t map(const xcb_intern_atom_reply_t &r) {
    return r.atom;
  }
};
} // namespace detail

using unique_xcb_generic_event_t = std::unique_ptr<xcb_generic_event_t, detail::c_free_deleter>;
using unique_xcb_generic_error_t = std::unique_ptr<xcb_generic_error_t, detail::c_free_deleter>;

class XcbColor;

struct XcbConnection final {
  XcbConnection(const char *name = NULL);
  XcbConnection(xcb_connection_t *conn, int default_screen_num = 0);
  ~XcbConnection();

  uint32_t generate_id() {
    return xcb_generate_id(conn);
  }

  void flush();
  // ? void sync(); -> xcb_aux_sync(conn); ?

  int fd() { // for polling
    return xcb_get_file_descriptor(conn);
  }

  operator xcb_connection_t *() {
    return conn;
  }

  // or XCB_ATOM_NONE (w/ create = false)
  XcbFuture<xcb_intern_atom_request_t, detail::intern_atom_atom> intern_atom(const char *name, bool create = false);
  XcbFuture<xcb_intern_atom_request_t, detail::intern_atom_atom> intern_atom(const char *name, uint16_t len, bool create = false) {
    return {conn, !create, len, name};
  }

  template <typename Fn>
  bool run_once(Fn&& fn) {
    while (auto ev = unique_xcb_generic_event_t{xcb_poll_for_event(conn)}) {
      if (ev->response_type == 0) {
        auto code = ((xcb_generic_error_t *)ev.get())->error_code;
        throw XcbEventError(code);
      } else if (!fn(ev.get())) {
        return false;
      }
    }
    return true;
  }

//  const xcb_setup_t *get_setup() const { return setup; }

  int screen_count() {
    return screen_cache.size(); // or: return xcb_setup_roots_length(setup);
  }

  int default_screen() {
    return default_screen_num;
  }

  xcb_screen_t *screen_of_display(int screen_num);
//  xcb_screen_t *screen(int screen_num) { return screen(screen_num); }
  xcb_screen_t *screen() {
    return screen_of_display(default_screen_num);
  }

  xcb_window_t root_window(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->root : 0;
  }
  xcb_window_t root_window() {
    return root_window(default_screen_num);
  }

  xcb_visualid_t default_visual(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->root_visual : 0;
  }
  xcb_visualid_t default_visual() {
    return default_visual(default_screen_num);
  }

  // returns (vt, depth) or (0,0)
  std::pair<const xcb_visualtype_t *, uint8_t> visualtype(xcb_screen_t *screen, xcb_visualid_t vid);
  std::pair<const xcb_visualtype_t *, uint8_t> visualtype(int screen_num, xcb_visualid_t vid) {
    xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? visualtype(screen, vid) : std::make_pair<const xcb_visualtype_t *, uint8_t>(0, 0);
  }
  std::pair<const xcb_visualtype_t *, uint8_t> default_visualtype(int screen_num) {
    xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? visualtype(screen, screen->root_visual) : std::make_pair<const xcb_visualtype_t *, uint8_t>(0, 0);
  }
  std::pair<const xcb_visualtype_t *, uint8_t> default_visualtype() {
    return default_visualtype(default_screen_num);
  }

  // (default_gc, default_gc_of_screen(int screen_num) ?)

  uint32_t black_pixel(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->black_pixel : 0;
  }
  uint32_t black_pixel() {
    return black_pixel(default_screen_num);
  }

  uint32_t white_pixel(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->white_pixel : 0;
  }
  uint32_t white_pixel() {
    return white_pixel(default_screen_num);
  }

  uint32_t display_width(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->width_in_pixels : 0;
  }
  uint32_t display_width() {
    return display_width(default_screen_num);
  }

  uint32_t display_height(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->height_in_pixels : 0;
  }
  uint32_t display_height() {
    return display_height(default_screen_num);
  }

  uint32_t display_width_mm(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->width_in_millimeters : 0;
  }
  uint32_t display_width_mm() {
    return display_width_mm(default_screen_num);
  }

  uint32_t display_height_mm(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->height_in_millimeters : 0;
  }
  uint32_t display_height_mm() {
    return display_height_mm(default_screen_num);
  }

  // display_planes / default_depth ?

  xcb_colormap_t default_colormap(int screen_num) {
    const xcb_screen_t *screen = screen_of_display(screen_num);
    return (screen) ? screen->default_colormap : 0;
  }
  xcb_colormap_t default_colormap() {
    return default_colormap(default_screen_num);
  }

  // event_mask_of_screen ??

  const xcb_format_t *format(uint8_t depth);

  // convenince (TODO? via XcbColormap wrapper?)
  XcbColor color(uint16_t red, uint16_t green, uint16_t blue); // (cmap = default_colormap());

private:
  void cache_screens();

private:
  xcb_connection_t *conn;
  bool owned;
  int default_screen_num;

  const xcb_setup_t *setup;
  std::vector<xcb_screen_t *> screen_cache;
};

class XcbColor final { // "unique XcbColor" resource wrapper
public:
  XcbColor(xcb_connection_t *conn, xcb_colormap_t cmap, uint32_t pixel) // "takes" pixel
    : conn(conn), cmap(cmap), pixel(pixel)
  { }

  ~XcbColor();

  XcbColor &operator=(const XcbColor &) = delete;

  operator uint32_t() {
    return pixel;
  }
private:
  xcb_connection_t *conn;
  xcb_colormap_t cmap;
  uint32_t pixel;
};


struct XcbWindow final {
  XcbWindow(
    XcbConnection &conn, xcb_window_t parent,
    uint16_t width, uint16_t height,
    uint32_t value_mask = 0, std::initializer_list<const uint32_t> value_list = {},
    uint16_t klass = XCB_WINDOW_CLASS_INPUT_OUTPUT,
    int16_t x = 0, int16_t y = 0, uint16_t border_width = 0,
    uint8_t depth = XCB_COPY_FROM_PARENT,
    xcb_visualid_t visual = XCB_COPY_FROM_PARENT);
  ~XcbWindow();

  XcbWindow(const XcbWindow &) = delete;

  std::pair<xcb_atom_t, xcb_atom_t> install_delete_handler();

  void map();
  void unmap();

  xcb_window_t get_window() { return win; }

  // attributes
  void change(uint32_t value_mask, std::initializer_list<const uint32_t> value_list);

private:
  XcbConnection &conn;
  xcb_window_t win;
};

struct XcbGC final {
  XcbGC(
    XcbConnection &conn,
    xcb_drawable_t drawable,
    uint32_t value_mask = 0, std::initializer_list<const uint32_t> value_list = {});

  ~XcbGC();

  XcbGC(const XcbGC &) = delete;

  void change(uint32_t value_mask, std::initializer_list<const uint32_t> value_list);

  operator xcb_gcontext_t () {
    return gc;
  }

private:
  XcbConnection &conn;
  xcb_gcontext_t gc;
};

