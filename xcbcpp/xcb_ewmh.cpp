#include "xcb_ewmh.h"

#define NET_WM_STATE_REMOVE   0u
#define NET_WM_STATE_ADD      1u
#define NET_WM_STATE_TOGGLE   2u

XcbEWMH::XcbEWMH(XcbConnection &conn)
  : conn(conn)
{
  auto nws = conn.intern_atom("_NET_WM_STATE");
  auto nwsf = conn.intern_atom("_NET_WM_STATE_FULLSCREEN");

  nwstate = nws.get();
  nwsfullscreen = nwsf.get();
}

void XcbEWMH::fullscreen(xcb_window_t win, bool val)
{
  send({
    XCB_CLIENT_MESSAGE, 32, 0, win,
    nwstate, {
      .data32 = {
        (val ? NET_WM_STATE_ADD : NET_WM_STATE_REMOVE),
        nwsfullscreen,
        0,    // second atom to change: 0: none
        1     // source: 1: normal application
      }
    }
  });
}

void XcbEWMH::send(const xcb_client_message_event_t &ev)
{
  xcb_send_event(  // (unchecked) - TODO?
    conn, 0,             // do not propagate
    conn.root_window(),  // dst  // TODO? per screen!?  - or: from win ?
    XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
    (const char *)&ev
  );
  conn.flush();
}

