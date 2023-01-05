#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <utility>
#include <vector>
#include <vector_types.h>
#include <algorithm>

#include "external/cuda-samples/helper_math.h"
#include "data_types.h"


/// Range of wavelengths visible to human eye
static const int lambdaMin = 400;
static const int lambdaMax = 700;
static const int numSpectralSamples = 60;

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

  std::transform(indices.begin(), indices.end(), lambda.begin(),
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


template <int nSpectrumSamples>
class CoefficientSpectrum {
  public:

  /// Default constructor
  __host__ __device__
  MySpectrum(Float v = 0.0f) {
    for (int i = 0; i < nSpectrumSamples; i++) {
      c[i] = v;
    }
  }

  /// Copy constructor
  __host__ __device__
  MySpectrum(const MySpectrum& other) {
    for (int i = 0; i < nSpectrumSamples; i++) {
      c[i] = other.c[i];
    }
  }

  /// Assignment operator
  __host__ __device__
  MySpectrum& operator=(const MySpectrum &other) {
    if (this != other) {
      for (int i = 0; i < nSpectrumSamples; i++) {
        c[i] = other.c[i];
      }
    }
    return this;
  }

  __host__ __device__
  MySpectrum& operator+=(const MySpectrum &other) {
    for (int i = 0; i < nSpectrumSamples; i++) {
      c[i] += other.c[i];
    }
    return *this;
  }
  __host__ __device__
  MySpectrum operator*=(const MySpectrum& other) {
    for (int i = 0; i < nSpectrumSamples; i++) {
      c[i] *= other.c[i];
    }
    return *this;
  }
  __host__ __device__
  MySpectrum operator+(const MySpectrum &other) {
    MySpectrum spec = *this;
    for (int i = 0; i < nSpectrumSamples; i++) {
      spec.c[i] += other.c[i];
    }
    return spec;
  }
  __host__ __device__
  MySpectrum operator*(const MySpectrum& other) {
    MySpectrum spec = *this;
    for (int i = 0; i < nSpectrumSamples; i++) {
      spec.c[i] *= other.c[i];
    }
    return spec;
  }

  __host__ __device__
  bool isBlack() {
    bool black = true;
    for (int i = 0; i < nSpectrumSamples; i++) {
      if (c[i] != Float(0)) {
	black = false;
	break;
      }
    }
    return black;
  }

  __host__ __device__
  Float totalPower() {
    Float specPower = Float(0);
    for (int i = 0; i < nSpectrumSamples; i++) {
      specPower += c[i];
    }
    return specPower;
  }

protected:
  Float c[nSpectrumSamples];
};

class VisibleSpectrum : public CoefficientSpectrum<numSpectralSamples> {
public:

  /// This constructor computes samples from piecewise linear function defined
  /// by samples (lambda,value)
  __host__ VisibleSpectrum(const std::vector<Float>& lambda,
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

    for (int i = 0; i < numSamples; i++) {
      c[i] = Float(0);
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

        Float b0 = lerp(l0, l1, minReg);
        Float b1 = lerp(l0, l1, maxReg);

	c[i] += (t0 + t1)/Float(2)*(b1 - b0);
      }
    }
  }
};

typedef VisibleSpectrum<numSpectralSamples> Spectrum;


class RGBSpectrum : public CoefficientSpectrum<3> {
  
};


inline void XYZToRGB(const Float xyz[3], Float rgb[3]) {
    rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
    rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
    rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
}


inline void RGBToXYZ(const Float rgb[3], Float xyz[3]) {
    xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
    xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
    xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
}

#endif//SPECTRUM_H
