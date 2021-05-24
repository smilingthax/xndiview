#pragma once

#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <stdexcept>
#include <memory>

// xcb_xkb_improved.h:
typedef union xkb_event_u xkb_event_t;

struct XkbError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

namespace detail {
struct xkb_context_deleter {
  void operator()(xkb_context *ctx) const {
    xkb_context_unref(ctx);
  }
};

struct xkb_keymap_deleter {
  void operator()(xkb_keymap *map) const {
    xkb_keymap_unref(map);
  }
};

struct xkb_state_deleter {
  void operator()(xkb_state *state) const {
    xkb_state_unref(state);
  }
};

struct xkb_compose_table_deleter {
  void operator()(xkb_compose_table *table) const {
    xkb_compose_table_unref(table);
  }
};

struct xkb_compose_state_deleter {
  void operator()(xkb_compose_state *state) const {
    xkb_compose_state_unref(state);
  }
};

} // namespace detail

using unique_xkb_context = std::unique_ptr<xkb_context, detail::xkb_context_deleter>;
using unique_xkb_keymap = std::unique_ptr<xkb_keymap, detail::xkb_keymap_deleter>;
using unique_xkb_state = std::unique_ptr<xkb_state, detail::xkb_state_deleter>;
using unique_xkb_compose_table = std::unique_ptr<xkb_compose_table, detail::xkb_compose_table_deleter>;
using unique_xkb_compose_state = std::unique_ptr<xkb_compose_state, detail::xkb_compose_state_deleter>;

struct XkbXcbExtensionBase {
  XkbXcbExtensionBase(xcb_connection_t *conn);

protected:
  uint8_t first_event_base, first_error_base;
};

struct XkbCompose {
  XkbCompose(xkb_context *ctx);
  XkbCompose(xkb_context *ctx, const char *locale,
    xkb_compose_compile_flags comp_flags = XKB_COMPOSE_COMPILE_NO_FLAGS,
    xkb_compose_state_flags state_flags = XKB_COMPOSE_STATE_NO_FLAGS);

  void reset();

  // returns final keysym, or XKB_KEY_NoSymbol when still incomplete
  xkb_keysym_t operator()(xkb_keysym_t in) {
    cur = process_one(in);
    return cur;
  }

  xkb_keysym_t current() {
    return cur;
  }

  // (c++20: std::u8string)
  std::string current_utf8();

  // NOTE: current_utf32 does not make sense, compose sequence result string can be of arbitrary length.

private:
  xkb_keysym_t process_one(xkb_keysym_t in);
  const char *_get_current_locale() const;
private:
  unique_xkb_compose_state state;
  xkb_keysym_t cur;
};

struct XkbXcbProcessor : XkbXcbExtensionBase {
  XkbXcbProcessor(xcb_connection_t *conn);  // uses core kbd
  XkbXcbProcessor(xcb_connection_t *conn, int32_t device_id);

  bool process(xkb_event_t *ev);
  bool process(xcb_generic_event_t *ev);

  xkb_keysym_t get_one_sym(xkb_keycode_t keycode);

xkb_context *get_context() const { return context.get(); } // FIXME

private:
  struct Impl;

  void _init();
  void update_keymap();
  void select_xkb_events() {}   // FIXME: TODO

private:
  xcb_connection_t *conn;
  int32_t device_id;

  unique_xkb_context context;
  unique_xkb_keymap keymap;
  unique_xkb_state state;
};

