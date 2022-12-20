#define BOOST_TEST_MODULE spectrum testcases 5
#include <boost/test/unit_test.hpp>

#include "spectrum.h"

BOOST_AUTO_TEST_SUITE(suite_spectrum)

BOOST_AUTO_TEST_CASE(spectrum_test1) {
  std::vector<Float> lambda = {Float(502), Float(507)};
  std::vector<Float> value = {Float(1), Float(1)};
  Spectrum spec(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), Float(5));
}

BOOST_AUTO_TEST_CASE(spectrum_test2) {
  std::vector<Float> lambda = {Float(502), Float(607)};
  std::vector<Float> value = {Float(1), Float(1)};
  Spectrum spec(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), Float(105));
}

BOOST_AUTO_TEST_CASE(spectrum_test3) {
  std::vector<Float> lambda = {Float(501), Float(504)};
  std::vector<Float> value = {Float(1), Float(0)};
  Spectrum spec(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), Float(1.5));
}

BOOST_AUTO_TEST_CASE(spectrum_test4) {
  std::vector<Float> lambda = {Float(504), Float(501), Float(507)};
  std::vector<Float> value = {Float(0), Float(1), Float(1)};
  Spectrum spec(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), Float(3));
}

BOOST_AUTO_TEST_CASE(spectrum_test5) {
  std::vector<Float> lambda = {Float(303), Float(804)};
  std::vector<Float> value = {Float(1), Float(1)};
  Spectrum spec(lambda, value);
  BOOST_CHECK_EQUAL(spec.totalPower(), Float(300));
}

BOOST_AUTO_TEST_SUITE_END()
