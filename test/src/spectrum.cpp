#define BOOST_TEST_MODULE spectrum testcases
#include <boost/test/unit_test.hpp>

#include "spectrum.h"

BOOST_AUTO_TEST_SUITE(suite_spectrum)

BOOST_AUTO_TEST_CASE(spectrum_test1) {
  std::vector<Float> lambda = {502, 507};
  std::vector<Float> value = {1, 1};
  Spectrum spec = Spectrum::createFromSpectrumArray(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), 5);
}

BOOST_AUTO_TEST_CASE(spectrum_test2) {
  std::vector<Float> lambda = {502, 607};
  std::vector<Float> value = {1, 1};
  Spectrum spec = Spectrum::createFromSpectrumArray(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), 105);
}

BOOST_AUTO_TEST_CASE(spectrum_test3) {
  std::vector<Float> lambda = {501, 504};
  std::vector<Float> value = {1, 0};
  Spectrum spec = Spectrum::createFromSpectrumArray(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), 1.5);
}

BOOST_AUTO_TEST_CASE(spectrum_test4) {
  std::vector<Float> lambda = {504, 501, 507};
  std::vector<Float> value = {0, 1, 1};
  Spectrum spec = Spectrum::createFromSpectrumArray(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), 3);
}

BOOST_AUTO_TEST_CASE(spectrum_test5) {
  std::vector<Float> lambda = {303, 804};
  std::vector<Float> value = {1, 1};
  Spectrum spec = Spectrum::createFromSpectrumArray(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), 300);
}

BOOST_AUTO_TEST_SUITE_END()
