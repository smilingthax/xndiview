#pragma once

#include "xcbevents.h"
#include <typeindex>

namespace detail {

struct pairhash {
  template <typename T1, typename T2>
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
  }
};


template <typename Sig, typename OnEmptyFn = void, typename ReduceFn = detail::reduce_void>
struct fltsig_base;

template <typename Sig, typename ReduceFn>
struct fltsig_base<Sig, void, ReduceFn> {
  fltsig_base()
    : signal(*this)
  { }

  void operator()() { // onempty
    conn.disconnect();
  }

protected:
  Signal<Sig, fltsig_base &, ReduceFn> signal;
  Connection conn;
};

template <typename Sig, typename OnEmptyFn, typename ReduceFn>
struct fltsig_base : fltsig_base<Sig, void, ReduceFn> {
  fltsig_base(OnEmptyFn&& onempty)
    : onempty((OnEmptyFn&&)onempty)
  { }

  void operator()() { // onempty
    fltsig_base::operator()();
    onempty();
  }

private:
  OnEmptyFn onempty;
};


struct signal_for_mem_base {
  virtual ~signal_for_mem_base() = default;
};

// OnConnectFn(Fn&&), OnEmptyFn()
template <typename EventType, typename T, T EventType::*Mem, typename OnConnectFn, typename OnEmptyFn>
class signal_for_mem final : public signal_for_mem_base {
  struct onempty_key {
    onempty_key(signal_for_mem &parent, const T &key)
      : parent(parent), key(key)
    { }

    void operator()() {
      parent.map.erase(key);
      if (parent.map.empty()) {
        parent.onempty();
      }
    }

    signal_for_mem &parent;
    T key;
  };

public:
  signal_for_mem(OnConnectFn&& onconnect = {}, OnEmptyFn&& onempty = {})
    : conn(onconnect(*this)),
      onempty(onempty)
  { }

  ~signal_for_mem() override {
    conn.disconnect();
  }

  void operator()(EventType *ev) {
    auto it = map.find(ev->*Mem);
    if (it != map.end()) {
      it->second.emit(ev);
    }
  }

  template <typename Fn>
  Connection connect(const T &key, Fn&& fn, SignalFlags flags = {}) {
    auto res = map.emplace(key, onempty_key{*this, key});
    try {
      return res.first->second.connect((Fn&&)fn, flags);
    } catch (...) {
      if (res.second) { // was inserted
        // onempty_key{*this, key}(); ...
        map.erase(key);
        if (map.empty()) {
          // conn.disconnect();
          onempty();
        }
      }
      throw;
    }
  }

private:
  std::unordered_map<T, Signal<void(EventType *), onempty_key>> map;
  Connection conn;
  OnEmptyFn onempty;
};

} // namespace detail

struct XcbDemux : XcbEventCallbacks {

#define MAKE_ONFN(Name, Event, Type, Mem) \
  template <typename Fn = void (*)(xcb_ ## Name ## _event_t *)>                                      \
  Connection on_ ## Name (Type val, Fn&& fn, SignalFlags flags = {}) {                               \
    return _connect_mem<XCB_ ## Event, Type, &xcb_ ## Name ## _event_t:: Mem>(val, (Fn&&)fn, flags); \
  }                                                                                                  \
  template <typename Fn = void (*)(xcb_ ## Name ## _event_t *),                                      \
            typename = typename std::enable_if<!std::is_same<Fn, Type>::value>::type>                \
  Connection on_ ## Name (Fn&& fn, SignalFlags flags = {}) {                                         \
    return XcbEventCallbacks::on<XCB_ ## Event>((Fn&&)fn, flags);                                    \
  }

  MAKE_ONFN(key_press, KEY_PRESS, xcb_window_t, event);
  MAKE_ONFN(key_release, KEY_RELEASE, xcb_window_t, event);
  MAKE_ONFN(button_press, BUTTON_PRESS, xcb_window_t, event);
  MAKE_ONFN(button_release, BUTTON_RELEASE, xcb_window_t, event);
  MAKE_ONFN(motion_notify, MOTION_NOTIFY, xcb_window_t, event);
  MAKE_ONFN(enter_notify, ENTER_NOTIFY, xcb_window_t, event);
  MAKE_ONFN(leave_notify, LEAVE_NOTIFY, xcb_window_t, event);
  MAKE_ONFN(focus_in, FOCUS_IN, xcb_window_t, event);
  MAKE_ONFN(focus_out, FOCUS_OUT, xcb_window_t, event);
  MAKE_ONFN(expose, EXPOSE, xcb_window_t, window);
  // ...
  MAKE_ONFN(configure_notify, CONFIGURE_NOTIFY, xcb_window_t, window);
  // ...
  MAKE_ONFN(client_message, CLIENT_MESSAGE, xcb_window_t, window);
#undef MAKE_ONFN

protected:
  using map_t = std::unordered_map<std::pair<uint8_t, std::type_index>, std::unique_ptr<detail::signal_for_mem_base>, detail::pairhash>;

  struct onempty {
    onempty(map_t &mem_map, const std::pair<uint8_t, std::type_index> &key)
      : mem_map(mem_map), key(key)
    { }

    void operator()() {
      mem_map.erase(key);
    }

    map_t &mem_map;
    std::pair<uint8_t, std::type_index> key;
  };

  template <typename Parent>
  struct connect_on_type {
    connect_on_type(Parent &parent, uint8_t type)
      : parent(parent), type(type)
    { }

    template <typename Fn>
    Connection operator()(Fn&& fn) {
      return parent.on(type, (Fn&&)fn);
    }

    Parent &parent;
    uint8_t type;
  };

  template <typename EventType,
            typename T, T EventType::*Mem,
            typename DoConnect,
            typename Fn = void (*)(EventType *)>
  Connection _connect_mem(uint8_t type, const T &val, Fn&& fn, SignalFlags flags, DoConnect&& doconnect) {
    // NOTE: we deliberately include DoConnect in type/typeid
    using sigmem_t = detail::signal_for_mem<EventType, T, Mem, DoConnect, onempty>;

    std::pair<uint8_t, std::type_index> key = { type, typeid(sigmem_t) };
    auto &res = mem_map[key];
    if (!res) { // was inserted
      try {
        std::unique_ptr<sigmem_t> sigmem{new sigmem_t((DoConnect&&)doconnect, {mem_map, key})};
        Connection ret = sigmem->connect(val, (Fn&&)fn, flags);
        res = std::move(sigmem);
        return ret;
      } catch (...) {
        mem_map.erase(key); // remove empty unique_ptr from map
        throw;
      }
    } else {
      sigmem_t &sigmem = static_cast<sigmem_t &>(*res); // safe, because map is keyed by typeid
      return sigmem.connect(val, (Fn&&)fn, flags);
    }
  }

  template <uint8_t Type,
            typename T, T detail::event_handler_type<Type>::type::*Mem,
            typename EventType = typename detail::event_handler_type<Type>::type,
            typename Fn = void (*)(EventType *)>
  Connection _connect_mem(const T &val, Fn&& fn, SignalFlags flags) {
    return _connect_mem<EventType, T, Mem, connect_on_type<XcbEventCallbacks>>(Type, val, (Fn&&)fn, flags, {*this, Type});
  }

  map_t mem_map;
};

