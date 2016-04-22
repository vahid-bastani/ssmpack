/**
 * @file traits.hpp
 * @author Vahid Bastani
 *
 * Generic class for first-order Markovian stochastic process.
 */
#ifndef SSMPACK_PROCESS_TRAITS_HPP
#define SSMPACK_PROCESS_TRAITS_HPP

#include "ssmpack/process/markov.hpp"

#include <type_traits>

namespace ssmpack {
namespace process {

template <typename T>
struct IsProcess : std::false_type {};

template <typename A, typename B>
struct IsProcess<Markov<A, B>> : std::true_type {};

template <typename A>
struct IsProcess<Memoryless<A>> : std::true_type {};
//==========================================
template <typename... Args>
struct AreProcesses;

template <typename T, typename... Args>
struct AreProcesses<T, Args...> {
  static constexpr bool value =
      AreProcesses<Args...>::value && IsProcess<T>::value;
};

template <>
struct AreProcesses<> {
  static constexpr bool value = true;
};
//=====================================================
template <typename T>
struct ModelTraits {};
template <typename R, typename C, typename... Args>
struct ModelTraits<R (C::*)(Args...)> {
  static constexpr size_t arity = sizeof...(Args);
};

//==========================================
template <typename T>
struct ProcessTraits {
  static constexpr bool valid = false;
  using TPDF = void;
  using TParamMap = void;
  using TRandomVAR = void;
  using type = T;
};

template <typename T, typename U, typename V>
struct ProcessTraits<Markov<distribution::Conditional<T, U>, V>> {
  static constexpr bool valid = true;
  using TPDF = T;
  using TParamMap = U;
  using TRandomVAR = decltype(std::declval<TPDF>().random());
  using type = Markov<distribution::Conditional<T, U>, V>;
  using TArity = std::integral_constant<
      size_t, ModelTraits<decltype(&TParamMap::operator())>::arity - 1>;
};

template <typename T, typename U>
struct ProcessTraits<Memoryless<distribution::Conditional<T, U>>> {
  static constexpr bool valid = true;
  using TPDF = T;
  using TParamMap = U;
  using TRandomVAR = decltype(std::declval<TPDF>().random());
  using type = Memoryless<distribution::Conditional<T, U>>;
  using TArity = std::integral_constant<
      size_t, ModelTraits<decltype(&TParamMap::operator())>::arity>;
};
}
}

#endif
