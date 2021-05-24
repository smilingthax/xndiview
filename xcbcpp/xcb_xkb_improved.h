#pragma once

// xcb/xkb.h has some quirks, fix them

// https://gitlab.freedesktop.org/xorg/lib/libxcb/-/issues/23
#define explicit c_explicit  // or: _explicit

#include <xcb/xkb.h>
#undef explicit

typedef union xkb_event_u {
  // based on xcb_raw_generic_event_t, but includes the common part of
  // new_keyboard_notify, map_notify and state_notify ...
  // - there is no official definition in an include file
  // -> private copy   (cf. libxkbcommon/tools/interactive-x11.c)
  struct {
      uint8_t response_type;
      uint8_t xkbType;       // .pad0 ...
      uint16_t sequence;
      xcb_timestamp_t time;  // .pad[0] ...
      uint8_t deviceID;
  } any;
  xcb_xkb_new_keyboard_notify_event_t new_keyboard_notify;
  xcb_xkb_map_notify_event_t map_notify;
  xcb_xkb_state_notify_event_t state_notify;
  xcb_xkb_controls_notify_event_t controls_notify;
  xcb_xkb_indicator_state_notify_event_t indicator_state_notify;
  xcb_xkb_indicator_map_notify_event_t indicator_map_notify;
  xcb_xkb_names_notify_event_t names_notify;
  xcb_xkb_compat_map_notify_event_t compat_map_notify;
  xcb_xkb_bell_notify_event_t bell_notify;
  xcb_xkb_action_message_event_t action_message;
  xcb_xkb_access_x_notify_event_t access_x_notify;
  xcb_xkb_extension_device_notify_event_t extension_device_notify;
} xkb_event_t;

