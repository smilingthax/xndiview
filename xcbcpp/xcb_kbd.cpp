#include "xcb_kbd.h"
#include "xcb_xkb_improved.h"  // instead of just <xcb/xkb.h>

template <typename Visitor>
static bool visit_xkb_event(xkb_event_t *ev, Visitor &&v)
{
  switch (ev->any.xkbType) {
  case XCB_XKB_NEW_KEYBOARD_NOTIFY:
    return v.process(&ev->new_keyboard_notify);
  case XCB_XKB_MAP_NOTIFY:
    return v.process(&ev->map_notify);
  case XCB_XKB_STATE_NOTIFY:
    return v.process(&ev->state_notify);
  case XCB_XKB_CONTROLS_NOTIFY:
    return v.process(&ev->controls_notify);
  case XCB_XKB_INDICATOR_STATE_NOTIFY:
    return v.process(&ev->indicator_state_notify);
  case XCB_XKB_INDICATOR_MAP_NOTIFY:
    return v.process(&ev->indicator_map_notify);
  case XCB_XKB_NAMES_NOTIFY:
    return v.process(&ev->names_notify);
  case XCB_XKB_COMPAT_MAP_NOTIFY:
    return v.process(&ev->compat_map_notify);
  case XCB_XKB_BELL_NOTIFY:
    return v.process(&ev->bell_notify);
  case XCB_XKB_ACTION_MESSAGE:
    return v.process(&ev->action_message);
  case XCB_XKB_ACCESS_X_NOTIFY:
    return v.process(&ev->access_x_notify);
  case XCB_XKB_EXTENSION_DEVICE_NOTIFY:
    return v.process(&ev->extension_device_notify);
  default:
    return false;
  }
}


struct XkbXcbProcessor::Impl {
  Impl(XkbXcbProcessor &p) : p(p) {}

  bool process(xcb_xkb_new_keyboard_notify_event_t *ev) {
    if (ev->changed & XCB_XKB_NKN_DETAIL_KEYCODES) {
      p.update_keymap();
    }
    return true;
  }

  bool process(xcb_xkb_map_notify_event_t *ev) {
    p.update_keymap();
    return true;
  }

  bool process(xcb_xkb_state_notify_event_t *ev) {
    xkb_state_update_mask(p.state.get(),
      ev->baseMods, ev->latchedMods, ev->lockedMods,
      ev->baseGroup, ev->latchedGroup, ev->lockedGroup);
    return true;
  }

  bool process(...) {
    return false;
  }

  static bool process(XkbXcbProcessor &p, xkb_event_t *ev) {
    if (ev->any.deviceID != p.device_id) {
      return false;
    }
    return visit_xkb_event(ev, Impl{p});
  }

private:
  XkbXcbProcessor &p;
};


XkbXcbExtensionBase::XkbXcbExtensionBase(xcb_connection_t *conn)
{
  // (idempotent)
  if (!xkb_x11_setup_xkb_extension(
    conn,
    XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION,
    XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS,
    NULL, NULL,
    &first_event_base, &first_error_base)) {
    // or: fallback to client side processing...  // TODO?
    throw XkbError("xkb_x11_setup_xkb_extension failed");
  }
}


XkbCompose::XkbCompose(xkb_context *ctx)
  : XkbCompose(ctx, _get_current_locale())
{
}

XkbCompose::XkbCompose(xkb_context *ctx, const char *locale,
  xkb_compose_compile_flags comp_flags,
  xkb_compose_state_flags state_flags)
  : cur(XKB_KEY_NoSymbol)
{
  unique_xkb_compose_table table{xkb_compose_table_new_from_locale(ctx, locale, comp_flags)};
  if (!table) {
    throw XkbError("xkb_compose_table_new_from_locale failed");
  }

  state.reset(xkb_compose_state_new(table.get(), state_flags));
  if (!state) {
    throw XkbError("xkb_compose_state_new failed");
  }
}

const char *XkbCompose::_get_current_locale() const
{
#if 0
  const char *ret = setlocale(LC_CTYPE, NULL);
#else
  const char *ret = getenv("LC_ALL");
  if (!ret || !*ret) {
    ret = getenv("LC_CTYPE");
    if (!ret || !*ret) {
      ret = getenv("LANG");
      // if (!ret || !*ret) ret = setlocale(LC_CTYPE, NULL); ???
    }
  }
#endif

  if (!ret || !*ret) {
    ret = "C";  // TODO?
  }
  return ret;
}

void XkbCompose::reset()
{
  xkb_compose_state_reset(state.get());
}

std::string XkbCompose::current_utf8()
{
  std::string ret;
  const int len = xkb_compose_state_get_utf8(state.get(), NULL, 0);
  if (len < 0) {
    throw std::runtime_error("xkb_compose_state_get_utf8 failed"); // TODO?
  }
  ret.resize(len);
  xkb_compose_state_get_utf8(state.get(), &ret[0], len); // (c++17: ret.data() [no longer const])
  return ret;
}

xkb_keysym_t XkbCompose::process_one(xkb_keysym_t in)
{
  switch (xkb_compose_state_feed(state.get(), in)) {
  case XKB_COMPOSE_FEED_IGNORED:
    return in;
  case XKB_COMPOSE_FEED_ACCEPTED:
    switch (xkb_compose_state_get_status(state.get())) {
    case XKB_COMPOSE_NOTHING:
      return in;
    case XKB_COMPOSE_COMPOSING:
      return XKB_KEY_NoSymbol;
    case XKB_COMPOSE_COMPOSED:
      return xkb_compose_state_get_one_sym(state.get());  // TODO? if NoSymbol: _get_utf8 ?
    case XKB_COMPOSE_CANCELLED:
      return XKB_KEY_NoSymbol;
    }
  }
  return XKB_KEY_NoSymbol; // (unreached)
}


// NOTE: cannot forward to device_id ctor, because ExtensionBase has to init xkb first!
XkbXcbProcessor::XkbXcbProcessor(xcb_connection_t *conn)
  : XkbXcbExtensionBase(conn), conn(conn), device_id(xkb_x11_get_core_keyboard_device_id(conn))
{
  _init();
}

XkbXcbProcessor::XkbXcbProcessor(xcb_connection_t *conn, int32_t device_id)
  : XkbXcbExtensionBase(conn), conn(conn), device_id(device_id)
{
  _init();
}

void XkbXcbProcessor::_init()
{
  context.reset(xkb_context_new(XKB_CONTEXT_NO_FLAGS));
  if (!context) {
    throw XkbError("xkb_context_new failed");
  }

  update_keymap();
  select_xkb_events();
}

void XkbXcbProcessor::update_keymap()
{
  xkb_keymap *map = xkb_x11_keymap_new_from_device(context.get(), conn, device_id, XKB_KEYMAP_COMPILE_NO_FLAGS);
  if (!map) {
    throw XkbError("xkb_x11_keymap_new_from_device failed");
  }
  keymap.reset(map);

  xkb_state *st = xkb_x11_state_new_from_device(map, conn, device_id);
  if (!st) {
    throw XkbError("xkb_x11_state_new_from_device failed");
  }
  state.reset(st);
}

bool XkbXcbProcessor::process(xkb_event_t *ev)
{
  return Impl::process(*this, ev);
}

bool XkbXcbProcessor::process(xcb_generic_event_t *ev)
{
  // TODO? (ev->response_type & ~0x80)  ?
  if (ev->response_type != first_event_base) {
    return false;
  }
  return process((xkb_event_t *)ev);
}

xkb_keysym_t XkbXcbProcessor::get_one_sym(xkb_keycode_t keycode)
{
  return xkb_state_key_get_one_sym(state.get(), keycode);
}

