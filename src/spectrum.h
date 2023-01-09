
/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <utility>
#include <vector>
#include <vector_types.h>
#include <algorithm>

#include "external/cuda-samples/helper_math.h"
#include "data_types.h"
#include "util/statusCode.h"


/// Range of wavelengths visible to human eye
static const int lambdaMin = 400;
static const int lambdaMax = 700;
static const int numVisibleSamples = 60;

enum class SpectrumType {Reflectance, Illuminant};

static bool pairComparison(const std::pair<int, Float>& a,
			   const std::pair<int, Float>& b);


void sortSpectrum(const std::vector<Float> &lambda,
                  const std::vector<Float> &value,
		  std::vector<Float> &sortedLambda,
		  std::vector<Float> &sortedValue);


template <int nSpectrumSamples>
class CoefficientSpectrum {
  public:

  /// Default constructor
  __host__ __device__
  CoefficientSpectrum(Float v = 0.0f) {
    for (int i = 0; i < nSpectrumSamples; i++) {
      c[i] = v;
    }
  }

  /// Copy constructor
  __host__ __device__
  CoefficientSpectrum(const CoefficientSpectrum& other) {
    for (int i = 0; i < nSpectrumSamples; i++) {
      c[i] = other.c[i];
    }
  }

  /// Assignment operator
  __host__ __device__
  CoefficientSpectrum& operator=(const CoefficientSpectrum &other) {
    if (*this != other) {
      for (int i = 0; i < nSpectrumSamples; i++) {
        c[i] = other.c[i];
      }
    }
    return *this;
  }

  __host__ __device__
  bool operator==(const CoefficientSpectrum &other) const {
    for (int i = 0; i < nSpectrumSamples; ++i) {
      if (c[i] != other.c[i]) {
        return false;
      }
    }
    return true;
  }

  __host__ __device__
  bool operator!=(const CoefficientSpectrum &other) const {
    return !(*this == other);
  }

  __host__ __device__
  CoefficientSpectrum& operator+=(const CoefficientSpectrum &other) {
    for (int i = 0; i < nSpectrumSamples; i++) {
      c[i] += other.c[i];
    }
    return *this;
  }
  __host__ __device__
  CoefficientSpectrum operator*=(const CoefficientSpectrum& other) {
    for (int i = 0; i < nSpectrumSamples; i++) {
      c[i] *= other.c[i];
    }
    return *this;
  }
  __host__ __device__
  CoefficientSpectrum operator+(const CoefficientSpectrum &other) {
    CoefficientSpectrum spec = *this;
    for (int i = 0; i < nSpectrumSamples; i++) {
      spec.c[i] += other.c[i];
    }
    return spec;
  }
  __host__ __device__
  CoefficientSpectrum operator*(const CoefficientSpectrum& other) {
    CoefficientSpectrum spec = *this;
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

  CoefficientSpectrum operator*(Float a) const {
    CoefficientSpectrum ret = *this;
    for (int i = 0; i < nSpectrumSamples; ++i) {
      ret.c[i] *= a;
    }
    return ret;
  }
  CoefficientSpectrum &operator*=(Float a) {
    for (int i = 0; i < nSpectrumSamples; ++i) {
      c[i] *= a;
    }
    return *this;
  }
  friend inline CoefficientSpectrum operator*(Float a,
					      const CoefficientSpectrum &s) {
    return s*a;
  }

protected:
  Float c[nSpectrumSamples];
};


static const int nCIESamples = 471;
extern const Float CIE_X[nCIESamples];
extern const Float CIE_Y[nCIESamples];
extern const Float CIE_Z[nCIESamples];
extern const Float CIE_lambda[nCIESamples];
static const int nRGB2SpectSamples = 32;
extern const Float RGB2SpectLambda[nRGB2SpectSamples];
extern const Float RGBRefl2SpectWhite[nRGB2SpectSamples];
extern const Float RGBRefl2SpectCyan[nRGB2SpectSamples];
extern const Float RGBRefl2SpectMagenta[nRGB2SpectSamples];
extern const Float RGBRefl2SpectYellow[nRGB2SpectSamples];
extern const Float RGBRefl2SpectRed[nRGB2SpectSamples];
extern const Float RGBRefl2SpectGreen[nRGB2SpectSamples];
extern const Float RGBRefl2SpectBlue[nRGB2SpectSamples];
extern const Float RGBIllum2SpectWhite[nRGB2SpectSamples];
extern const Float RGBIllum2SpectCyan[nRGB2SpectSamples];
extern const Float RGBIllum2SpectMagenta[nRGB2SpectSamples];
extern const Float RGBIllum2SpectYellow[nRGB2SpectSamples];
extern const Float RGBIllum2SpectRed[nRGB2SpectSamples];
extern const Float RGBIllum2SpectGreen[nRGB2SpectSamples];
extern const Float RGBIllum2SpectBlue[nRGB2SpectSamples];


class VisibleSpectrum : public CoefficientSpectrum<numVisibleSamples> {
public:

  /// This constructor computes samples from piecewise linear function defined
  /// by samples (lambda,value)
  static StatusCode initSpectralData() {
    StatusCode status = StatusCode::NoError;
    std::vector<Float> CIE_lambda_vec(CIE_lambda, CIE_lambda + nCIESamples);
    std::vector<Float> CIE_X_vec(CIE_X, CIE_X + nCIESamples);
    std::vector<Float> CIE_Y_vec(CIE_Y, CIE_Y + nCIESamples);
    std::vector<Float> CIE_Z_vec(CIE_Z, CIE_Z + nCIESamples);

    X = createFromSpectrumArray(CIE_X_vec, CIE_lambda_vec);
    Y = createFromSpectrumArray(CIE_Y_vec, CIE_lambda_vec);
    Z = createFromSpectrumArray(CIE_Z_vec, CIE_lambda_vec);

    std::vector<Float> RGB2SpectLambda_vec(RGB2SpectLambda,
					   RGB2SpectLambda + nRGB2SpectSamples);
    std::vector<Float> RGBRefl2SpectWhite_vec(RGBRefl2SpectWhite,
					      RGBRefl2SpectWhite + nRGB2SpectSamples);
    std::vector<Float> RGBRefl2SpectCyan_vec(RGBRefl2SpectCyan,
					     RGBRefl2SpectCyan + nRGB2SpectSamples);
    std::vector<Float> RGBRefl2SpectMagenta_vec(RGBRefl2SpectMagenta,
						RGBRefl2SpectMagenta + nRGB2SpectSamples);
    std::vector<Float> RGBRefl2SpectYellow_vec(RGBRefl2SpectYellow,
					       RGBRefl2SpectYellow + nRGB2SpectSamples);
    std::vector<Float> RGBRefl2SpectRed_vec(RGBRefl2SpectRed,
					    RGBRefl2SpectRed + nRGB2SpectSamples);
    std::vector<Float> RGBRefl2SpectGreen_vec(RGBRefl2SpectGreen,
					      RGBRefl2SpectGreen + nRGB2SpectSamples);
    std::vector<Float> RGBRefl2SpectBlue_vec(RGBRefl2SpectBlue,
					     RGBRefl2SpectBlue + nRGB2SpectSamples);
    std::vector<Float> RGBIllum2SpectWhite_vec(RGBIllum2SpectWhite,
					       RGBIllum2SpectWhite + nRGB2SpectSamples);
    std::vector<Float> RGBIllum2SpectCyan_vec(RGBIllum2SpectCyan,
					      RGBIllum2SpectCyan + nRGB2SpectSamples);
    std::vector<Float> RGBIllum2SpectMagenta_vec(RGBIllum2SpectMagenta,
						 RGBIllum2SpectMagenta + nRGB2SpectSamples);
    std::vector<Float> RGBIllum2SpectYellow_vec(RGBIllum2SpectYellow,
						RGBIllum2SpectYellow + nRGB2SpectSamples);
    std::vector<Float> RGBIllum2SpectRed_vec(RGBIllum2SpectRed,
					     RGBIllum2SpectRed + nRGB2SpectSamples);
    std::vector<Float> RGBIllum2SpectGreen_vec(RGBIllum2SpectGreen,
					       RGBIllum2SpectGreen + nRGB2SpectSamples);
    std::vector<Float> RGBIllum2SpectBlue_vec(RGBIllum2SpectBlue,
					      RGBIllum2SpectBlue + nRGB2SpectSamples);

    rgbRefl2SpectWhite = createFromSpectrumArray(RGB2SpectLambda_vec, RGBRefl2SpectWhite_vec);
    rgbRefl2SpectCyan = createFromSpectrumArray(RGB2SpectLambda_vec, RGBRefl2SpectCyan_vec);
    rgbRefl2SpectMagenta = createFromSpectrumArray(RGB2SpectLambda_vec, RGBRefl2SpectMagenta_vec);
    rgbRefl2SpectYellow = createFromSpectrumArray(RGB2SpectLambda_vec, RGBRefl2SpectYellow_vec);
    rgbRefl2SpectRed = createFromSpectrumArray(RGB2SpectLambda_vec, RGBRefl2SpectRed_vec);
    rgbRefl2SpectGreen = createFromSpectrumArray(RGB2SpectLambda_vec, RGBRefl2SpectGreen_vec);
    rgbRefl2SpectBlue = createFromSpectrumArray(RGB2SpectLambda_vec, RGBRefl2SpectBlue_vec);

    rgbIllum2SpectWhite = createFromSpectrumArray(RGB2SpectLambda_vec, RGBIllum2SpectWhite_vec);
    rgbIllum2SpectCyan = createFromSpectrumArray(RGB2SpectLambda_vec, RGBIllum2SpectCyan_vec);
    rgbIllum2SpectMagenta = createFromSpectrumArray(RGB2SpectLambda_vec, RGBIllum2SpectMagenta_vec);
    rgbIllum2SpectYellow = createFromSpectrumArray(RGB2SpectLambda_vec, RGBIllum2SpectYellow_vec);
    rgbIllum2SpectRed = createFromSpectrumArray(RGB2SpectLambda_vec, RGBIllum2SpectRed_vec);
    rgbIllum2SpectGreen = createFromSpectrumArray(RGB2SpectLambda_vec, RGBIllum2SpectGreen_vec);
    rgbIllum2SpectBlue = createFromSpectrumArray(RGB2SpectLambda_vec, RGBIllum2SpectBlue_vec);

    return status;
  }

  static VisibleSpectrum createFromSpectrumArray(
						 const std::vector<Float>& lambda,
						 const std::vector<Float>& value) {
    VisibleSpectrum s;
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

    for (int i = 0; i < numVisibleSamples; i++) {
      s.c[i] = Float(0);
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

      while (idx0 < numVisibleSamples && specLambda0 < l0) {
	idx0++;
	specLambda0 = lerp(lambdaMin, lambdaMax, Float(idx0)/numVisibleSamples);
      }
      idx0--;
      while (idx1 < numVisibleSamples && specLambda1 < l1) {
      	idx1++;
	specLambda1 = lerp(lambdaMin, lambdaMax, Float(idx1)/numVisibleSamples);
      }
      idx1--;

      Float v0 = sortedValue[i];
      Float v1 = sortedValue[i+1];

      const Float dc = (lambdaMax - lambdaMin)/numVisibleSamples;

      for (int i = idx0; i <= idx1; i++) {
	Float specLambda = lerp(lambdaMin, lambdaMax, Float(i)/numVisibleSamples);
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

	s.c[i] += (t0 + t1)/Float(2)*(b1 - b0);
      }
    }
    return s;
  }

  static VisibleSpectrum fromRGB(const Float rgb[3],
				 SpectrumType type);

private:
  static VisibleSpectrum X, Y, Z;
  static VisibleSpectrum rgbRefl2SpectWhite, rgbRefl2SpectCyan;
  static VisibleSpectrum rgbRefl2SpectMagenta, rgbRefl2SpectYellow;
  static VisibleSpectrum rgbRefl2SpectRed, rgbRefl2SpectGreen;
  static VisibleSpectrum rgbRefl2SpectBlue;
  static VisibleSpectrum rgbIllum2SpectWhite, rgbIllum2SpectCyan;
  static VisibleSpectrum rgbIllum2SpectMagenta, rgbIllum2SpectYellow;
  static VisibleSpectrum rgbIllum2SpectRed, rgbIllum2SpectGreen;
  static VisibleSpectrum rgbIllum2SpectBlue;
};


// inline void XYZToRGB(const Float xyz[3], Float rgb[3]) {
//     rgb[0] = 3.240479f * xyz[0] - 1.537150f * xyz[1] - 0.498535f * xyz[2];
//     rgb[1] = -0.969256f * xyz[0] + 1.875991f * xyz[1] + 0.041556f * xyz[2];
//     rgb[2] = 0.055648f * xyz[0] - 0.204043f * xyz[1] + 1.057311f * xyz[2];
// }


// class RGBSpectrum : public CoefficientSpectrum<3> {
//   using CoefficientSpectrum<3>::c;

// public:
//   /// Default constructor
//   /// Call parent class constructior
//   RGBSpectrum(Float v = 0.f) : CoefficientSpectrum<3>(v) {}

//   static RGBSpectrum createFromRGBFloats(const Float rgb[3])
//   {
//     RGBSpectrum s;
//     s.c[0] = rgb[0];
//     s.c[1] = rgb[1];
//     s.c[2] = rgb[2];
//     return s;
//   }

//   static RGBSpectrum createFromXYZFloats(const Float xyz[3])
//   {
//     RGBSpectrum s;
//     XYZToRGB(xyz, s.c);
//     return s;
//   }
// };


// inline void RGBToXYZ(const Float rgb[3], Float xyz[3]) {
//     xyz[0] = 0.412453f * rgb[0] + 0.357580f * rgb[1] + 0.180423f * rgb[2];
//     xyz[1] = 0.212671f * rgb[0] + 0.715160f * rgb[1] + 0.072169f * rgb[2];
//     xyz[2] = 0.019334f * rgb[0] + 0.119193f * rgb[1] + 0.950227f * rgb[2];
// }


typedef VisibleSpectrum Spectrum;

#endif//SPECTRUM_H
