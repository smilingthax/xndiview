#pragma once

#include "xcb_base.h"
#include <xcb/shm.h>

// NOTE: only for XCB_IMAGE_FORMAT_Z_PIXMAP, depth \in { 24, 32 }
struct XcbImage {
  XcbImage(XcbConnection &conn, xcb_drawable_t drawable, uint8_t depth);

  XcbImage &operator=(const XcbImage &) = delete;

  bool has(size_t _width, size_t _height) const {
    return (width == _width && height == _height);
  }
  size_t stride() const {
    return row_stride;
  }
  void *data();
  void *data(size_t width, size_t height);

  // NOTE: visual/layout depends on drawable, drawable depth MUST match this->depth
  void put(xcb_drawable_t drawable, int16_t dst_x = 0, int16_t dst_y = 0);

private:
  // XcbConnection &conn;  // (store only once in ShmSegment)
  XcbGC gc;
  const xcb_format_t *fmt;
  uint32_t max_req_len; // in 32-bit words

  struct ShmSegment {
    ShmSegment(XcbConnection &conn);
    ~ShmSegment();

    ShmSegment &operator=(const ShmSegment &) = delete;

    // TODO... moveable

    bool reset(size_t size = 0);

    size_t size() const {
      return len;
    }
    void *get() { // NULL when not available or size()==0
      return address;
    }

    xcb_shm_get_image_cookie_t get_image(
      xcb_drawable_t drawable,
      int16_t x, int16_t y, uint16_t width, uint16_t height,
      uint32_t plane_mask = ~0,
      uint8_t format = XCB_IMAGE_FORMAT_Z_PIXMAP,
      uint32_t offset = 0);

    xcb_void_cookie_t put_image_checked(
      xcb_drawable_t drawable,
      xcb_gcontext_t gc,
      uint16_t total_width, uint16_t total_height,
      uint16_t src_x, uint16_t src_y,
      uint16_t src_width, uint16_t src_height,
      int16_t dst_x, int16_t dst_y,
      uint8_t depth,
      uint8_t format = XCB_IMAGE_FORMAT_Z_PIXMAP,
      bool send_event = false,
      uint32_t offset = 0);

    XcbConnection &conn;
  private:
    std::unique_ptr<xcb_shm_query_version_reply_t, detail::c_free_deleter> version;

    void *address;
    size_t len;

    xcb_shm_seg_t segment;

#if (XCB_SHM_MAJOR_VERSION == 1 && XCB_SHM_MINOR_VERSION >= 2) || XCB_SHM_MAJOR_VERSION > 1
    bool use_mmap;
#endif
  };
  ShmSegment shm;

  size_t width, height, row_stride;
  std::vector<uint8_t> buf;  // if not shm
  std::vector<xcb_void_cookie_t> ckcache; // keep memory around
};

