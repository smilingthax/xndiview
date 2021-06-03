#include "xcb_img.h"
#include <unistd.h>
#include <sys/shm.h>
#if (XCB_SHM_MAJOR_VERSION == 1 && XCB_SHM_MINOR_VERSION >= 2) || XCB_SHM_MAJOR_VERSION > 1
#  include <sys/mman.h>
#endif

namespace detail {
// NOTE: expect this to not be needed anywhere else
XCB_MAKE_REQ_TRAIT(shm_query_version);

struct fd_array {
  fd_array(int *ptr, size_t size)
    : data(ptr), len(size)
  { }

  ~fd_array() {
    if (data) {
      for (size_t i = 0; i < len; i++) {
        close(data[i]);
      }
      // NOT: free(data);  -> docu of xcb_shm_create_segment_reply_fds is wrong, only the reply shall be freed.
      // -> https://gitlab.freedesktop.org/xorg/lib/libxcb/-/issues/56
    }
  }

  size_t size() const {
    return len;
  }

  fd_array &operator=(const fd_array &) = delete;

  explicit operator bool() const {
    return !!data;
  }

  int operator[](size_t pos) const {
    // assert(pos < len);
    return data[pos];
  }

private:
  int *data;
  size_t len;
};
} // namespace detail


XcbImage::XcbImage(
  XcbConnection &conn, xcb_drawable_t drawable, uint8_t depth)
  : gc(conn, drawable, XCB_GC_GRAPHICS_EXPOSURES, { 0 }),
    fmt(conn.format(depth)),
    max_req_len(xcb_get_maximum_request_length(conn)),
    shm(conn),
    width(0), height(0), row_stride(0)
{
  if (!fmt) {
    throw std::runtime_error("Format/Depth not available");  // FIXME/TODO
  }
}

void *XcbImage::data()
{
  void *ret = shm.get();
  if (ret) {
    return ret;
  }
  if (!buf.empty()) {
    return buf.data();
  }
  // assert(row_stride == 0 && height == 0);
  return nullptr;
}

void *XcbImage::data(size_t _width, size_t _height)
{
  width = _width;
  height = _height;
  row_stride = ((fmt->bits_per_pixel * width) & -fmt->scanline_pad) >> 3;

  const size_t size = row_stride * height;

  if (shm.size() >= size) { // reuse existing shm; also handles size==0
    // assert(buf.empty());
    // NOTE: memory is NOT cleared
    return shm.get();
  } else if (shm.reset(size)) { // shm create successful
    if (!buf.empty()) {
      buf.clear();
      buf.shrink_to_fit(); // TODO?
    }
    return shm.get();
  } // else: fallback

  buf.resize(size);
  return buf.data();
}

void XcbImage::put(xcb_drawable_t drawable, int16_t dst_x, int16_t dst_y)
{
  void *addr = shm.get();
  if (addr) {
    xcb_void_cookie_t ck = shm.put_image_checked(
      drawable, gc,
      width, height,
      0, 0, width, height,
      dst_x, dst_y,
      fmt->depth,
      XCB_IMAGE_FORMAT_Z_PIXMAP);

    // also waits for server to finish op
    unique_xcb_generic_error_t error{xcb_request_check(shm.conn, ck)};
    if (error) {
      throw XcbGenericError(error->error_code);  // TODO? just return false?
    }

  } else if (!buf.empty()) {
    // assert(buf.size() >= row_stride * height);
    const size_t max_rows = (4 * max_req_len - sizeof(xcb_put_image_request_t)) / row_stride;
    const size_t num_stripes = (height + max_rows - 1) / max_rows;

    ckcache.resize(num_stripes);
    for (size_t i = 0, pos = 0; i < num_stripes; i++, pos += max_rows) {
      ckcache[i] = xcb_put_image_checked(
        shm.conn, XCB_IMAGE_FORMAT_Z_PIXMAP,
        drawable, gc,
        width, std::min(height - pos, max_rows),
        dst_x, dst_y + pos,
        0,   // left_pad  (TODO?)
        fmt->depth,
        buf.size() - pos, // row_stride * height,
        buf.data() + pos);
    }

    for (size_t i = 0; i < num_stripes; i++) {
      unique_xcb_generic_error_t error{xcb_request_check(shm.conn, ckcache[i])};
      if (error) {
        throw XcbGenericError(error->error_code);
      }
    }

  } else {
    // TODO ??? (if width > 0 && height > 0 error; ?)
  }
}

XcbImage::ShmSegment::ShmSegment(XcbConnection &conn)
  : conn(conn), address(nullptr), len(0), segment(-1)
{
  const xcb_query_extension_reply_t *ext = xcb_get_extension_data(conn, &xcb_shm_id); // (cached, compared to raw xcb_query_extension())
  if (!ext || !ext->present) {
    // fprintf(stderr, "MIT-SHM extension not present. (%p)\n", ext);
    return;  // TODO? option: fail when shm is not available?
  }

  XcbFuture<xcb_shm_query_version_request_t> ver(conn);
  version = ver.get();

#if (XCB_SHM_MAJOR_VERSION == 1 && XCB_SHM_MINOR_VERSION >= 2) || XCB_SHM_MAJOR_VERSION > 1
  use_mmap = (version->major_version == 1 && version->minor_version >= 2) || version->major_version > 2;
#endif
}

XcbImage::ShmSegment::~ShmSegment()
{
  if (address) {
    xcb_shm_detach(conn, segment);
#if (XCB_SHM_MAJOR_VERSION == 1 && XCB_SHM_MINOR_VERSION >= 2) || XCB_SHM_MAJOR_VERSION > 1
    if (use_mmap) {
      munmap(address, len);
      return;
    }
#endif
    shmdt(address);
  }
}

// NOTE: size can be a "max size", i.e. bigger than the actual image size
bool XcbImage::ShmSegment::reset(size_t size)
{
  if (!version) {
    // assert(!address);
    return false;
  }

  if (address) {
    // if (len == size) return true;

    xcb_shm_detach(conn, segment);  // TODO? checked?
#if (XCB_SHM_MAJOR_VERSION == 1 && XCB_SHM_MINOR_VERSION >= 2) || XCB_SHM_MAJOR_VERSION > 1
    if (use_mmap) {
      munmap(address, len); // TODO? check retval?
    } else {
      shmdt(address);
    }
#else
    shmdt(address);
#endif

    address = nullptr;
    len = 0;
    segment = -1;
  }

  if (!size) {
    return true;
  }

#if (XCB_SHM_MAJOR_VERSION == 1 && XCB_SHM_MINOR_VERSION >= 2) || XCB_SHM_MAJOR_VERSION > 1
  if (use_mmap) {
    xcb_shm_seg_t seg = conn.generate_id();
    xcb_shm_create_segment_cookie_t ck = xcb_shm_create_segment(conn, seg, size, 0); // readonly=1);  ?

    // NOTE: not XcbFuture<...>, because we want to fallback on error
    xcb_shm_create_segment_reply_t *reply = xcb_shm_create_segment_reply(conn, ck, NULL);
    if (!reply) {
      // TODO? handle certain errors specially?
      return false;
    }

    detail::fd_array fds{xcb_shm_create_segment_reply_fds(conn, reply), reply->nfd};
    ::free(reply);

    if (!fds || fds.size() != 1) {
      // fprintf(stderr, "Unexpected: Successful shm_create_segment did not return single fd\n");
      return false;
    }

    void *addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fds[0], 0);
    if (addr == MAP_FAILED) {
      // fprintf(stderr, "mmap failed in XcbImage::ShmSegment::reset()\n");
      xcb_shm_detach(conn, seg);  // TODO? checked?

      throw std::bad_alloc();
    }

    address = addr;
    len = size;
    segment = seg;

    return true;
  }
#endif
  // fprintf(stderr, "fallback to non-create_segment shm\n");

  const int id = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);
  if (id == -1) {
    return false;
  }

  void *addr = shmat(id, 0, 0);
  if (address == (void *)-1) {
    shmctl(id, IPC_RMID, NULL);  // TODO? look at retval ?
    return false;
  }

  xcb_shm_seg_t seg = conn.generate_id();
  xcb_void_cookie_t ck = xcb_shm_attach_checked(conn, seg, id, 0);
  xcb_generic_error_t *error = xcb_request_check(conn, ck);

  // CAVE: on non-linux, this shall not happen before xcb_shm_attach has executed in the server!
  shmctl(id, IPC_RMID, NULL);   // TODO? look at retval?

  if (error) {
    shmdt(address);
    // handle certain errors specially?
    return false;
  } // else: assume no connection errors?

  address = addr;
  len = size;
  segment = seg;

  return true;
}

xcb_shm_get_image_cookie_t XcbImage::ShmSegment::get_image(
  xcb_drawable_t drawable,
  int16_t x, int16_t y,
  uint16_t width, uint16_t height,
  uint32_t plane_mask,
  uint8_t format,
  uint32_t offset)
{
  // assert(address && segment != -1);
  // assert(len >= offset + stride * height); // w/ stride = ...
  return xcb_shm_get_image(
    conn, drawable,
    x, y, width, height,
    plane_mask, format,
    segment, offset);
}

xcb_void_cookie_t XcbImage::ShmSegment::put_image_checked(
  xcb_drawable_t drawable,
  xcb_gcontext_t gc,
  uint16_t total_width, uint16_t total_height,
  uint16_t src_x, uint16_t src_y,
  uint16_t src_width, uint16_t src_height,
  int16_t dst_x, int16_t dst_y,
  uint8_t depth,
  uint8_t format,
  bool send_event,
  uint32_t offset)
{
  // assert(address && segment != -1);
  // assert(len >= offset + stride * src_height); // w/ stride = ...
  return xcb_shm_put_image_checked(
    conn, drawable, gc,
    total_width, total_height,
    src_x, src_y, src_width, src_height,
    dst_x, dst_y,
    depth, format,
    (send_event ? 1 : 0),
    segment, offset);
}

