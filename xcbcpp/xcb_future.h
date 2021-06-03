#pragma once

#include "xcb_base.h"  // esp.: xcb_proto.tcc
#include <memory>

namespace detail {
struct wrap_unique_c_free {}; // sentinel type
} // namespace detail

template <typename T, typename ReplyMapOp = detail::wrap_unique_c_free>
class XcbFuture;

template <typename T>
class XcbFuture<T, detail::wrap_unique_c_free> {
  using Trait = detail::XcbRequestTraits<T>;
public:
  template <typename... Args>
  XcbFuture(xcb_connection_t *conn, Args&&... args)
    : conn(conn) {
    cookie = Trait::request(conn, (Args&&)args...);
  }

  std::unique_ptr<typename Trait::reply_t, detail::c_free_deleter> get() {
    xcb_generic_error_t *error = NULL;
    std::unique_ptr<typename Trait::reply_t, detail::c_free_deleter> reply{Trait::get(conn, cookie, &error)};
    if (reply) {
      // TODO?! cookie is no longer "valid" (get() cannot be called twice!)
      return reply;
    }

    if (!error) {
      const int res = xcb_connection_has_error(conn);
      if (res) {
        throw XcbConnectionError(res);
      }
    }
    // assert(error);
    const int code = error->error_code;
    free(error);
    throw XcbGenericError(std::string("XcbFuture<") + Trait::name + "> error", code);
  }

private:
  xcb_connection_t *conn;
  typename Trait::cookie_t cookie;
};

template <typename T, typename ReplyMapOp>
class XcbFuture : XcbFuture<T, detail::wrap_unique_c_free> {
  using Trait = detail::XcbRequestTraits<T>;
  using reply_t = decltype(ReplyMapOp::map(*(const typename Trait::reply_t *)0));
public:
  using XcbFuture<T, detail::wrap_unique_c_free>::XcbFuture;

  reply_t get() {
    auto reply = XcbFuture<T, detail::wrap_unique_c_free>::get();
    return ReplyMapOp::map(*reply);
  }
};

