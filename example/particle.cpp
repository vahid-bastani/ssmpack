#include "ssmpack/filter/particle.hpp"
#include "ssmpack/map/linear_gaussian.hpp"
#include "ssmpack/distribution/gaussian.hpp"
#include "ssmpack/distribution/conditional.hpp"
#include "ssmpack/process/markov.hpp"
#include "ssmpack/process/memoryless.hpp"
#include "ssmpack/process/hierarchical.hpp"

#include <iostream>

using namespace ssmpack;

struct dummy_resampler {
  template <class A, class B>
  void operator()(A &a, B &b) {}
};

int main () {
  double delta = 0.1; // sample time
  arma::mat::fixed<4, 4> dynamic_matrix{
      {1, 0, delta, 0}, {0, 1, 0, delta}, {0, 0, 1, 0}, {0, 0, 0, 1}};

  arma::mat::fixed<4, 4> dynamic_noise{
      {0.1, 0, 0, 0}, {0, 0.1, 0, 0}, {0, 0, 0.1, 0}, {0, 0, 0, 0.1}};

  arma::mat::fixed<2, 4> measurement_matrix{{1, 0, 0, 0}, {0, 1, 0, 0}};
  arma::mat::fixed<2, 2> measurement_noise{{0.1, 0}, {0, 0.1}};

  auto dynamic_model = map::makeLinearGaussian(dynamic_matrix, dynamic_noise);
  auto measurement_model =
      map::makeLinearGaussian(measurement_matrix, measurement_noise);

  auto dynamic_cpdf =
      distribution::makeConditional(distribution::Gaussian<4>(), dynamic_model);
  auto measurement_cpdf = distribution::makeConditional(
      distribution::Gaussian<2>(), measurement_model);

  auto state_process =
      process::makeMarkov(dynamic_cpdf, distribution::Gaussian<4>());
  auto measurement_process =
      process::makeMemoryless(measurement_cpdf);

  auto joint_process =
      process::makeHierarchical(state_process, measurement_process);

  auto pfilter = filter::makeParticle(joint_process, dummy_resampler(), 10);

  random::setRandomSeed();

  pfilter.initialize();

  std::cout << pfilter.getWeights();
  std::vector<typename decltype(joint_process)::TRandomVARs> v;
  joint_process.initialize();
  joint_process.random_n(v, 100);
  std::vector<typename std::tuple_element<1,decltype(v)::value_type>::type> m(100);
  int count=0;
  std::generate_n(m.begin(), 100,
                  [&count, &v]() { return std::get<1>(v[count++]); });
  auto o = pfilter.filter(m);
  return 0;
}