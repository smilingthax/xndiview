#include "myui.h"

MyUI::MyUI(const char *name, int width, int height, bool gray)
  : conn(),
    bgcol(gray ? conn.color(0x7f7f, 0x7f7f, 0x7f7f) : conn.color(0, 0, 0)),  // (note: cannot just use conn.black_pixel(), because XcbColor frees)
    win(conn, conn.root_window(), width, height,
      XCB_CW_EVENT_MASK, {
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_STRUCTURE_NOTIFY
      }),
    gc(conn, win.get_window(), XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES, { bgcol, 0 }),
    dmux(win.install_delete_handler()),
    ewmh(conn),
    img(conn, win.get_window(), 24),
    img_width(0), img_height(0)
{
  dmux.on_key_press(win.get_window(), [this](xcb_key_press_event_t *ev) {
// printf("0x%x 0x%x\n", ev->detail, ev->state);
    if (ev->detail == 0x18) done = true; // 'q' ...        // FIXME?!
    else if (ev->detail == 0x29) { fullscr ^= 1; fullscreen(fullscr); } // 'f' ...        // FIXME?!
  });

  // set window title
  std::string title = name + std::string(" - xndiview");  // (NOTE: use esp. _NET_WM_NAME [-> XcbEWMH] for utf8 ...)
  xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win.get_window(), XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.size(), title.data());

  dmux.on_wm_delete(win.get_window(), [this](xcb_client_message_event_t *ev) {
    done = true;
  });

  init_image();

  dmux.on_configure_notify(win.get_window(), [this](xcb_configure_notify_event_t *ev) {
    // assert(ev->event == ev->windows);  // i.e.: not SubstructureNotify
    img_width = ev->width;
    img_height = ev->height;
  });

  dmux.on_expose(win.get_window(), [this](xcb_expose_event_t *ev) {
    if (ev->count != 0) return;

    if (cur.data) {
      do_draw(true);
    } else {
      xcb_rectangle_t r = {0, 0, img_width, img_height};
      xcb_poly_fill_rectangle(conn, win.get_window(), gc, 1, &r);  // (unchecked)
      conn.flush();
    }
  });

  win.map();
}

void MyUI::init_image()
{
  std::pair<const xcb_visualtype_t *, uint8_t> vtd = conn.default_visualtype();
  if (!vtd.first ||
      vtd.second != 24 ||
      vtd.first->_class != XCB_VISUAL_CLASS_TRUE_COLOR ||
      vtd.first->bits_per_rgb_value != 8 ||    // ? colormap_entries = 256 ?
      vtd.first->red_mask != 0xff0000 ||
      vtd.first->green_mask != 0x00ff00 ||
      vtd.first->blue_mask != 0x0000ff) {   // --> NOTE: corresponds to BGRX in ndi
    throw std::runtime_error("Unsupported visualtype");
  }

  const xcb_format_t *vf = conn.format(vtd.second);
  if (!vf ||
      vf->bits_per_pixel != 32) {
    throw std::runtime_error("Unsupported format for depth");
  }
}

bool MyUI::run_once()
{
  return conn.run_once([this](xcb_generic_event_t *ev) {
    dmux.emit(ev);
    return !done;
  });
}

#include "libyuv/scale_argb.h"
#include <assert.h>

struct imgfit_t {
  imgfit_t(int sw, int sh, int width, int height);  // mode = "contain"

  int dx, dy, dw, dh;
};

imgfit_t::imgfit_t(int sw, int sh, int width, int height)
  : dx(0), dy(0), dw(width), dh(height)
{
  if ((float)sw * height < (float)sh * width) { // height-constrained
    dw = sw * dh / sh;
    dx = (width - dw) / 2;
  } else { // width-constrained
    dh = sh * dw / sw;
    dy = (height - dh) / 2;
  }
}

void MyUI::do_draw(bool clear)
{
  if (img_width == 0 || img_height == 0) {
    return;
  }

  const imgfit_t fit{cur.xres, cur.yres, img_width, img_height};
  uint8_t *dst;
  if (!img.has(fit.dw, fit.dh)) {
    clear = true;
    dst = (uint8_t *)img.data(fit.dw, fit.dh);
  } else {
    dst = (uint8_t *)img.data();
  }
  const int res = libyuv::ARGBScale(
    cur.data, cur.stride, cur.xres, cur.yres,
    dst, img.stride(), fit.dw, fit.dh,
    libyuv::kFilterBilinear);
  assert(res == 0);

  if (transparency) { // TODO/FIXME: SSSE3, AVX2, NEON, ... version ? ...  // TODO? elsewhere ?
    // + pre-multiplies (Attenuate)
    uint8_t *row = dst;
    for (int y = 0; y < fit.dh; y++) {
      uint8_t *rgba = row;
      for (int x = 0; x < fit.dw; x++) {
        if (rgba[3] == 0xff) {
          rgba += 4;
          continue;
        }
        const float alpha = rgba[3] / 255.0f,
                    ialpha = 1.0f - alpha;
        const uint8_t bgcol = ((x / 8 + y / 8) & 1) ? 0xbb : 0xff;
        rgba[0] = (uint8_t)(rgba[0] * alpha + bgcol * ialpha);  // TODO? clamp ?
        rgba[1] = (uint8_t)(rgba[1] * alpha + bgcol * ialpha);
        rgba[2] = (uint8_t)(rgba[2] * alpha + bgcol * ialpha);
        rgba += 4;
      }
      row += img.stride();
    }
  }

  if (clear) {
#if 0
    xcb_rectangle_t r = {0, 0, img_width, img_height};
    xcb_poly_fill_rectangle(conn, win.get_window(), gc, 1, &r);
#else
    xcb_rectangle_t r;
    r = {0, 0, (uint16_t)fit.dx, img_height};
    xcb_poly_fill_rectangle(conn, win.get_window(), gc, 1, &r);
    r = {0, 0, img_width, (uint16_t)fit.dy};
    xcb_poly_fill_rectangle(conn, win.get_window(), gc, 1, &r);
    r = {(int16_t)(fit.dx + fit.dw), 0, img_width, img_height};
    xcb_poly_fill_rectangle(conn, win.get_window(), gc, 1, &r);
    r = {0, (int16_t)(fit.dy + fit.dh), img_width, img_height};
    xcb_poly_fill_rectangle(conn, win.get_window(), gc, 1, &r);
#endif
  }
  img.put(win.get_window(), fit.dx, fit.dy);
}

