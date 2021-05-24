#pragma once

#include <type_traits>

namespace detail {

template <typename... Args>
struct typelist_t;

template <typename Arg0, typename... Args>
struct typelist_t<Arg0, Args...> {
  static constexpr const unsigned int size = sizeof...(Args) + 1;

  template <unsigned int N, typename = void>
  struct get : typelist_t<Args...>::template get<N-1> { };

  template <typename Dummy> // explicit specialization not allowed, but partial is...
  struct get<0, Dummy> {
    using type = Arg0;
  };
};

template <typename Fn>
struct get_args_hlp_t;

template <typename R, typename... Args>
struct get_args_hlp_t<R(Args...)> {
  using types = typelist_t<R, Args...>;
};

template <typename C, typename R, typename... Args>
struct get_args_hlp_t<R(C::*)(Args...)> {
  using types = typelist_t<R, Args...>;
};

template <typename C, typename R, typename... Args>
struct get_args_hlp_t<R(C::*)(Args...) const> {
  using types = typelist_t<R, Args...>;
};

template <typename Fn>
auto get_args_hlp(Fn& fn)
  -> typename get_args_hlp_t<decltype(&Fn::operator())>::types
{ return {}; }

template <typename Fn>
auto get_args_hlp(Fn& fn)
  -> typename get_args_hlp_t<typename std::remove_pointer<Fn>::type>::types
{ return {}; }

} // namespace detail

template <typename Fn>
using get_args_t = decltype(detail::get_args_hlp(*(typename std::remove_reference<Fn>::type*)0));

template <unsigned int N, typename Fn>
using get_arg_t = typename get_args_t<Fn>::template get<N>::type;

