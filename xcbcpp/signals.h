#pragma once

#include <memory>

// NOTE: During an active emit connections MUST NOT be removed, nor .clear() be called!

struct Connection;

enum SignalFlags {
  SIGNAL_DEFAULT = 0,
  SIGNAL_ONCE    = 0x01,
  SIGNAL_PREPEND = 0x02
};

namespace detail {

struct SignalConnectionBase {
  virtual ~SignalConnectionBase();
  virtual void remove_node() = 0;

  ::Connection *conns = {};
};

template <typename Sig>
struct SignalListBase;

template <typename Ret, typename... Args>
struct SignalListBase<Ret(Args...)> : SignalConnectionBase {
  SignalListBase() = default;
  SignalListBase(SignalListBase &&) = delete;  // TODO?

  virtual Ret operator()(Args...) = 0;

  virtual void setNext(std::unique_ptr<SignalListBase> &node) {
    next.swap(node);
  }

  void remove_node() override {
    // assert(prev);
    if (next) {
      next->prev = prev;
    }

    std::unique_ptr<SignalListBase> tmp = std::move(next);
    prev->setNext(tmp);  // tmp now holds *this and keeps it alive until the current scope ends
    // ~SignalConnectionBase() will now disarm the connections
  }

  SignalListBase *prev = {};
  std::unique_ptr<SignalListBase> next;
  bool once = false;  // unused in SignalListRoot, but shall be accessible with only Base*
};

template <typename Sig, typename Fn>
struct SignalListNode;

template <typename Ret, typename... Args, typename Fn>
struct SignalListNode<Ret(Args...), Fn> final : SignalListBase<Ret(Args...)> {
  SignalListNode(Fn&& fn, bool _once) : fn((Fn&&)fn) {
    SignalListBase<Ret(Args...)>::once = _once;
  }

  Ret operator()(Args... args) override {
    return fn(args...);
  }

  Fn fn;
};

template <typename... Args, typename Fn>
struct SignalListNode<void(Args...), Fn> final : SignalListBase<void(Args...)> {
  SignalListNode(Fn&& fn, bool _once) : fn((Fn&&)fn) {
    SignalListBase<void(Args...)>::once = _once;
  }

  void operator()(Args... args) override {
    fn(args...);
  }

  Fn fn;
};

template <typename Sig, typename OnEmptyFn = void>
struct SignalListRoot final : SignalListRoot<Sig, void> {
  using base_t = SignalListBase<Sig>;

  SignalListRoot(OnEmptyFn& onempty)
    : onempty(onempty)
  { }

  void clear() {
    if (base_t::next) {
      // SignalListRoot<Sig, void>::clear();
      base_t::next.reset();
      base_t::prev = nullptr;
      onempty();
    }
  }

  void setNext(std::unique_ptr<base_t> &node) override {
    // base_t::setNext(node);
    base_t::next.swap(node);
    if (!base_t::next) {
      onempty();
    }
  }

private:
  OnEmptyFn onempty;
};

template <typename Ret, typename... Args>
struct SignalListRoot<Ret(Args...), void> : SignalListBase<Ret(Args...)> {
  void clear() {
    using base_t = SignalListBase<Ret(Args...)>;
    base_t::next.reset();
    base_t::prev = nullptr;
  }
  Ret operator()(Args...) override {
    throw 0;
  }
  void remove_node() override {
    throw 0;
  }
//  using base_t::setNext;
};

enum class reduce_result_t : uint8_t {
  CONTINUE,
  REMOVE_HANDLER,
  STOP
};

struct reduce_void { // {{{
  reduce_result_t operator()(...) {
    return reduce_result_t::CONTINUE;
  }
  void get() { }
};
// }}}

template <typename T>
struct reduce_use_last { // {{{
  reduce_use_last(T init = {}) : last(std::move(init)) {}

  reduce_result_t operator()(T next) {
    last = std::move(next);
    return reduce_result_t::CONTINUE;
  }

  T get() { return last; }

  T last;
};

template <>
struct reduce_use_last<void> : reduce_void { };
// }}}

} // namespace detail

struct Connection final {
  Connection()
    : node(), prev(), next()
  { }

  ~Connection() {
    if (node) {
      unlink();
    }
  }

  Connection(const Connection &rhs) = delete;
  Connection(Connection &&rhs)
    : Connection()
  {
    *this = std::move(rhs);
  }

//  Connection &operator=(const Connection &rhs) = delete;
  Connection &operator=(Connection &&rhs) {
    disconnect();

    node = move(rhs.node);
    prev = move(rhs.prev);
    next = move(rhs.next);

    if (node) {
      if (next) {
        next->prev = &next;
      }
      *prev = this;
    }

    return *this;
  }

  void disconnect() {
    if (node) {
      unlink();
      node->remove_node();
      node = nullptr;
    }
  }

  bool connected() const {
    return node;
  }

private:
  template <typename Sig, typename OnEmptyFn, typename DefaultRetvalFn>
  friend class Signal;
  friend struct detail::SignalConnectionBase;

  Connection(detail::SignalConnectionBase *node)
    : node(node), prev(&node->conns), next(node->conns) {
    node->conns = this;
  }

  void unlink() {
    // assert(node && prev);
    if (next) {
      next->prev = prev;
    }
    *prev = next;
  }

  template <typename T>
  T move(T &t, T&& new_val = {}) {
    T ret = std::move(t);
    t = (T&&)new_val;
    return ret;
  }

  detail::SignalConnectionBase *node;
  Connection **prev, *next;
};

inline detail::SignalConnectionBase::~SignalConnectionBase()
{
  while (conns) {
    conns->node = nullptr;
    conns = conns->next;
  }
}

//template <typename Sig, typename OnEmptyFn = void, typename DefaultRetvalFn = detail::reduce_void>
template <typename Sig, typename OnEmptyFn = void, typename DefaultRetvalFn = detail::reduce_use_last<void>>
class Signal;

template <typename Ret, typename... Args, typename OnEmptyFn, typename DefaultRetvalFn>
class Signal<Ret(Args...), OnEmptyFn, DefaultRetvalFn> final {
  using Sig = Ret(Args...);
  using base_t = detail::SignalListBase<Sig>;
public:
  Signal() = default;

  // template to avoid error when OnEmptyFn = void
  template <typename Fn = OnEmptyFn,
            typename = typename std::enable_if<std::is_same<Fn, OnEmptyFn>::value>::type>
  Signal(Fn& onempty) : root{onempty} { }

  template <typename Fn = OnEmptyFn,
            typename = typename std::enable_if<std::is_same<Fn, OnEmptyFn>::value>::type>
  Signal(Fn&& onempty) : root{onempty} { }

  void clear() {
    root.clear();
  }

  bool empty() const {
    return !root.next;
  }

  template <typename Fn>
  Connection prepend(Fn&& fn, bool once = false) {
    auto node = new detail::SignalListNode<Sig, Fn>{(Fn&&)fn, once};
    if (root.next) {
      root.next->prev = node;
      node->next = std::move(root.next);
    } else {
      root.prev = node;
    }
    root.next.reset(node);
    return {node};
  }

  template <typename Fn>
  Connection append(Fn&& fn, bool once = false) {
    auto node = new detail::SignalListNode<Sig, Fn>{(Fn&&)fn, once};
    if (root.prev) {
      // assert(!root.prev->next);
      root.prev->next.reset(node);
      node->prev = root.prev;
      // NOTE: node->next must stay empty, because root is already owned
    } else {
      root.next.reset(node);
      node->prev = &root;
    }
    root.prev = node;
    return {node};
  }

  template <typename Fn>
  Connection connect(Fn&& fn, SignalFlags flags = {}) {
    return (flags & SIGNAL_PREPEND) ? prepend((Fn&&)fn, flags & SIGNAL_ONCE) : append((Fn&&)fn, flags & SIGNAL_ONCE);
  }

  template <typename ReduceRetvalFn = DefaultRetvalFn,
            typename = typename std::enable_if<!std::is_void<Ret>::value, ReduceRetvalFn>::type>
  auto emit(Args... args) -> decltype(((ReduceRetvalFn *)0)->get()) {
    ReduceRetvalFn ret;
    for (base_t *node = root.next.get(); node; node = node->next.get()) {
      const detail::reduce_result_t res = ret((*node)(args...));
      if (res == detail::reduce_result_t::REMOVE_HANDLER || node->once) {
        node = node->prev;
        node->next->remove_node();
      }
      if (res == detail::reduce_result_t::STOP) {
        return ret.get();
      }
    }
    return ret.get();
  }

  template <typename ReduceRetvalFn = DefaultRetvalFn,
            typename = typename std::enable_if<std::is_void<Ret>::value, ReduceRetvalFn>::type>
  void emit(Args... args) {
    for (base_t *node = root.next.get(); node; node = node->next.get()) {
      (*node)(args...);
      if (node->once) {
        node = node->prev;
        node->next->remove_node();
      }
    }
  }

private:
  detail::SignalListRoot<Sig, OnEmptyFn> root;
};

