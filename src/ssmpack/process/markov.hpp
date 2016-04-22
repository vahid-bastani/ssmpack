/**
 * @file markov.hpp
 * @author Vahid Bastani
 *
 * Generic class for first-order Markovian stochastic process.
 */
#ifndef SSMPACK_PROCESS_MARKOV_HPP
#define SSMPACK_PROCESS_MARKOV_HPP

#include "ssmpack/distribution/conditional.hpp"
#include "ssmpack/process/base_process.hpp"

#include <algorithm>
#include <cmath>
#include <type_traits>
#include <vector>

namespace ssmpack {
namespace process {

/**
 * A first-order Markov process
 */
template <typename TTransitionCPDF, typename TInitialPDF>
class Markov {};

template <typename TPDF, typename TParamMap, typename TInitialPDF>
class Markov<distribution::Conditional<TPDF, TParamMap>, TInitialPDF>
    : public BaseProcess<Markov<distribution::Conditional<TPDF, TParamMap>,
    TInitialPDF>> {
 public:
  Markov(distribution::Conditional<TPDF, TParamMap> cpdf, TInitialPDF init_pdf)
      : cpdf_(std::move(cpdf)), init_pdf_(std::move(init_pdf)) {}

  auto initialize() -> decltype(std::declval<TPDF>().random()) {
    state_ = init_pdf_.random();
    return state_;
  }

  template <typename... Args>
  auto random(const Args &... args) -> decltype(std::declval<TPDF>().random()) {
    state_ = cpdf_.random(state_, args...);
    return state_; // why not returning reference?
  }

  template <typename... Args>
  double likelihood(const decltype(std::declval<TPDF>().random()) &rv,
                    const Args &... args) {
    return cpdf_.likelihood(rv, state_, args...);
  }

 private:
  distribution::Conditional<TPDF, TParamMap> cpdf_;
  TInitialPDF init_pdf_;
  decltype(std::declval<TPDF>().random()) state_;
};

template <typename TPDF, typename TParamMap, typename TInitialPDF>
Markov<distribution::Conditional<TPDF, TParamMap>, TInitialPDF>
makeMarkov(distribution::Conditional<TPDF, TParamMap> cpdf,
           TInitialPDF init_pdf) {
  return Markov<distribution::Conditional<TPDF, TParamMap>, TInitialPDF>(
      cpdf, init_pdf);
}

} // namespace process
} // namespace ssmpack

#endif // SSMPACK_PROCESS_MARKOV_HPP