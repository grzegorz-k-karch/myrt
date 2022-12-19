#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <utility>
#include <vector>
#include <vector_types.h>
#include <algorithm>
#include <iostream>

#include "external/cuda-samples/helper_math.h"
#include "data_types.h"


static bool pairComparison(const std::pair<int, Float>& a,
			   const std::pair<int, Float>& b)
{
  return a.second < b.second;
}

void sortSpectrum(const std::vector<Float> &lambda,
                  const std::vector<Float> &value,
		  std::vector<Float> &sortedLambda,
		  std::vector<Float> &sortedValue)
{
  std::vector<int> indices(lambda.size());
  for (int i = 0; i < lambda.size(); i++) {
    indices[i] = i;
  }
  std::vector<std::pair<int, Float>> enum_lambda;
  enum_lambda.reserve(lambda.size());

  std::transform(lambda.begin(), lambda.end(), indices.begin(),
		 std::back_inserter(enum_lambda),
		 [](int a, Float b) {return std::make_pair(a,b);});
  std::sort(enum_lambda.begin(), enum_lambda.end(), pairComparison);

  sortedValue.reserve(lambda.size());
  sortedLambda.reserve(lambda.size());
  for (auto& el : enum_lambda) {
    int idx = el.first;
    sortedValue.push_back(value[idx]);
    sortedLambda.push_back(lambda[idx]);
  }
}


template <int numSamples>
class MySpectrum {
public:

  /// Range of wavelengths visible to human eye
  static const int lambdaMin = 400;
  static const int lambdaMax = 700;

  /// Default constructor
  __host__ __device__
  MySpectrum(Float v = 0.0f) {
    for (int i = 0; i < numSamples; i++) {
      c[i] = v;
    }
  }
  /// Copy constructor
  __host__ __device__
  MySpectrum(const MySpectrum& other) {
    for (int i = 0; i < numSamples; i++) {
      c[i] = other.c[i];
    }
  }

  /// This constructor computes samples from piecewise linear function defined
  /// by samples (lambda,value)
  __host__ MySpectrum(const std::vector<Float>& lambda,
		      const std::vector<Float>& value) {
    std::vector<Float> sortedLambda;
    std::vector<Float> sortedValue;
    bool lambdaIsSorted = std::is_sorted(lambda.begin(), lambda.end());
    if (!lambdaIsSorted) {
      sortSpectrum(lambda, value, sortedLambda, sortedValue);
    }
    else {
      sortedLambda = lambda;
      sortedValue = value;
    }

    const int len = lambda.size();
    for (int i = 0; i < len-1; i++) {
      Float l0 = sortedLambda[i];
      Float l1 = sortedLambda[i+1];

      /// Find start/end indices of c intervals overlapped by piece (l0,l1).
      /// In case when idx0 == idx1 a piece is within one interval.
      /// Figure 5.2 from pbr-book
      int idx0 = 0;
      int idx1 = 0;
      Float specLambda0 = lambdaMin;
      Float specLambda1 = lambdaMin;

      while (idx0 < numSamples && specLambda0 < l0) {
	idx0++;
	specLambda0 = lerp(lambdaMin, lambdaMax, Float(idx0)/numSamples);
      }
      idx0--;
      while (idx1 < numSamples && specLambda1 < l1) {
      	idx1++;
	specLambda1 = lerp(lambdaMin, lambdaMax, Float(idx1)/numSamples);
      }
      idx1--;

      Float v0 = sortedValue[i];
      Float v1 = sortedValue[i+1];

      const Float dc = (lambdaMax - lambdaMin)/numSamples;

      for (int i = idx0; i <= idx1; i++) {
	Float specLambda = lerp(lambdaMin, lambdaMax, Float(i)/numSamples);
      	/// Compute location of current coefficient range relative to
      	/// piece (l0, l1).
      	Float a = (specLambda - l0)/(l1 - l0);
        Float b = (specLambda + dc - l0)/(l1 - l0);

        Float minReg = std::max(a, Float(0));
        Float maxReg = std::min(b, Float(1));

        Float t0 = lerp(v0, v1, minReg);
        Float t1 = lerp(v0, v1, maxReg);

	c[i] += (t0 + t1)/Float(2)*(maxReg - minReg);
      }
    }
    for (auto& cc : c) {
      std::cout << cc << std::endl;
    }
  }


  /// Assignment operator
  __host__ __device__
  MySpectrum& operator=(const MySpectrum &other) {
    if (this != other) {
      for (int i = 0; i < numSamples; i++) {
        c[i] = other.c[i];
      }
    }
    return this;
  }
  __host__ __device__
  MySpectrum& operator+=(const MySpectrum &other) {
    for (int i = 0; i < numSamples; i++) {
      c[i] += other.c[i];
    }
    return *this;
  }
  __host__ __device__
  MySpectrum operator*=(const MySpectrum& other) {
    for (int i = 0; i < numSamples; i++) {
      c[i] *= other.c[i];
    }
    return *this;
  }
  __host__ __device__
  MySpectrum operator+(const MySpectrum &other) {
    MySpectrum spec = *this;
    for (int i = 0; i < numSamples; i++) {
      spec.c[i] += other.c[i];
    }
    return spec;
  }
  __host__ __device__
  MySpectrum operator*(const MySpectrum& other) {
    MySpectrum spec = *this;
    for (int i = 0; i < numSamples; i++) {
      spec.c[i] *= other.c[i];
    }
    return spec;
  }

  __host__ __device__
  bool isBlack() {
    bool black = true;
    for (int i = 0; i < numSamples; i++) {
      if (c[i] != Float(0)) {
	black = false;
	break;
      }
    }
    return black;
  }

private:
  Float c[numSamples];
};

static const int numSpectralSamples = 60;

typedef MySpectrum<numSpectralSamples> Spectrum;


#endif//SPECTRUM_H
