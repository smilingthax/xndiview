#pragma once

#include "signals.h"
#include "xcb_base.h" // esp. for xcb_proto.tcc detail::event_handler_type
#include <unordered_map>
#include "getargtype.h"

struct XcbEventCallbacks {
  template <typename EventType, typename Fn = void (*)(EventType *)>
  Connection on(uint8_t type, Fn&& fn, SignalFlags flags = {}) {
    return _connect<EventType *>(type, (Fn&&)fn, flags);
  }

  template <uint8_t type,
            typename EventType = typename detail::event_handler_type<type>::type,
            typename Fn = void (*)(EventType *)>
  Connection on(Fn&& fn, SignalFlags flags = {}) {
    return _connect<EventType *>(type, (Fn&&)fn, flags);
  }

  template <typename Fn, typename EventTypePtr = get_arg_t<1, Fn>>
  Connection on(uint8_t type, Fn&& fn, SignalFlags flags = {}) {
    return _connect<EventTypePtr>(type, (Fn&&)fn, flags);
  }

  void emit(uint8_t type, xcb_generic_event_t *ev) const {
    auto it = map.find(type);
    if (it == map.end()) {
      return;
    }
    it->second->emit(ev);
  }

  void emit(xcb_generic_event_t *ev) const {
    emit(ev->response_type & ~0x80, ev);   // TODO?
  }

private:
  struct TypedEventSignalBase {
    virtual ~TypedEventSignalBase() = default;
    virtual void emit(xcb_generic_event_t *ev) = 0;
  };

  template <typename EventTypePtr>
  struct TypedEventSignal final : TypedEventSignalBase {
    TypedEventSignal(XcbEventCallbacks &parent, uint8_t type)
      : parent(parent), type(type), signal(*this)
    { }

    void emit(xcb_generic_event_t *ev) override {
      signal.emit((EventTypePtr)ev);
    }

    void operator()() const { // onempty
      parent.map.erase(type);
    }

    template <typename Fn>
    Connection connect(Fn&& fn, SignalFlags flags) {
      return signal.connect((Fn&&)fn, flags);
    }

  private:
    XcbEventCallbacks &parent;
    uint8_t type;

    Signal<void(EventTypePtr), TypedEventSignal &, detail::reduce_void> signal;
  };

  template <typename EventTypePtr, typename Fn>
  Connection _connect(uint8_t type, Fn&& fn, SignalFlags flags) {
    auto &res = map[type];
    if (!res) { // was inserted
      try {
        std::unique_ptr<TypedEventSignal<EventTypePtr>> signal{new TypedEventSignal<EventTypePtr>{*this, type}};
        Connection ret = signal->connect((Fn&&)fn, flags);
        res = std::move(signal);
        return ret;
      } catch (...) {
        map.erase(type); // remove empty unique_ptr from map
        throw;
      }
    } else {
#if 1
      auto &tmp = *res;  // avoid clang warning for  typeid(*res)
      if (typeid(TypedEventSignal<EventTypePtr>&) != typeid(tmp)) {
        throw std::bad_cast();
      }
      TypedEventSignal<EventTypePtr> &signal = static_cast<TypedEventSignal<EventTypePtr> &>(tmp);
#else
      TypedEventSignal<EventTypePtr> &signal = dynamic_cast<TypedEventSignal<EventTypePtr> &>(*res);
#endif
      return signal.connect((Fn&&)fn, flags);
    }
  }

  std::unordered_map<uint8_t, std::unique_ptr<TypedEventSignalBase>> map;
};

