#pragma once

#include "xcb_base.h"

// -> https://specifications.freedesktop.org/wm-spec/wm-spec-latest.html

// TODO? only request those atoms really needed?
class XcbEWMH {
public:
  XcbEWMH(XcbConnection &conn);

  void fullscreen(xcb_window_t win, bool val);   // TODO? root window per screen?

private:
  XcbConnection &conn;
  xcb_atom_t nwstate, nwsfullscreen;

  void send(const xcb_client_message_event_t &ev);
};

