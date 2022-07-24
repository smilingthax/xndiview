#pragma once

#include "xcbcpp/xcb_ewmh.h"
#include "xcbcpp/xcb_img.h"

#include "xcbcpp/xcbdemuxwm.h"

class MyUI {
public:
  MyUI(const char *name = "", int width = 480, int height = 270, bool gray = false);
  MyUI(const char *name, bool gray) : MyUI(name, 480, 270, gray) {}

  bool run_once();

  void draw(const uint8_t *data, int stride, int xres, int yres) {
    // assert(data);
    cur.data = data;
    cur.stride = stride;
    cur.xres = xres;
    cur.yres = yres;
    do_draw(false);
  }

  void fullscreen(bool val) {
    fullscr = val;
    ewmh.fullscreen(win.get_window(), val);
  }

  void show_transparency(bool val) {
    transparency = val;
    do_draw(false);
  }

  void close() { // TODO?
    win.unmap();
  }

private:
  XcbConnection conn;
  XcbColor bgcol;
  XcbWindow win;
  XcbGC gc;
  XcbDemuxWithWM dmux;

  XcbEWMH ewmh;

  XcbImage img;
  void init_image();
  uint16_t img_width, img_height;

  bool transparency = false;

  struct {
    const uint8_t *data;
    int stride, xres, yres;
  } cur = { 0 };
  void do_draw(bool clear);

  bool fullscr = false;
  bool done = false;
};

