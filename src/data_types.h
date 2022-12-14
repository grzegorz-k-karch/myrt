#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <vector_types.h>
#include <vector>
#include <algorithm>
#include <utility>

typedef float Float;

/// Entities without any suffix consist of three floats by default
/// (equivalen of 3f suffix)

struct MyPoint {
  union {
    Float p[3];
    Float x, y, z;
  };
};


struct MyVector {
  union {
    Float d[3];
    Float x, y, z;
  };
};


struct MyNormal {
  union {
    Float d[3];
    Float x, y, z;
  };
};


typedef MyPoint Point;
typedef MyVector Vector;
typedef MyNormal Normal;


static bool myComparison(const std::pair<int, Float>& a,
			 const std::pair<int, Float>& b)
{
  return a.second < b.second;
}


template <int numSamples>
class MySpectrum {
public:

  /// Range of wavelengths visible to human eye
  static const int lambdaMin = 400;
  static const int lambdaMax = 700;

  /// Default constructor
  MySpectrum(Float v = 0.0f) {
    for (int i = 0; i < numSamples; i++) {
      c[i] = v;
    }
  }
  /// Copy constructor
  MySpectrum(const MySpectrum& other) {
    for (int i = 0; i < numSamples; i++) {
      c[i] = other.c[i];
    }
  }

  /// This constructor computes samples from piecewise linear function defined
  /// by samples (lambda,value)
  MySpectrum(const std::vector<Float>& lambda,
	     const std::vector<Float>& value) {
    if (!sorted(lambda)) {
      std::vector<int> indices(lambda.size());
      for (int i = 0; i < lambda.size(); i++) {
	indices[i] = i;
      }
      std::vector<std::pair<int, Float>> enum_lambda;

      enum_lambda.reserve(lambda.size());
      std::transform(lambda.begin(), lambda.end(), indices.begin(),
                     std::back_inserter(enum_lambda),
		     [](int a, Float b) {return std::make_pair(a,b);});
      std::sort(enum_lambda.begin(), enum_lambda.end(), myComparison);
    }
  }

  /// Assignment operator
  MySpectrum& operator=(const MySpectrum &other) {
    if (this != other) {
      for (int i = 0; i < numSamples; i++) {
        c[i] = other.c[i];
      }
    }
    return this;
  }
  MySpectrum& operator+=(const MySpectrum &other) {
    for (int i = 0; i < numSamples; i++) {
      c[i] += other.c[i];
    }
    return *this;
  }
  MySpectrum operator*=(const MySpectrum& other) {
    for (int i = 0; i < numSamples; i++) {
      c[i] *= other.c[i];
    }
    return *this;
  }
  MySpectrum operator+(const MySpectrum &other) {
    MySpectrum spec = *this;
    for (int i = 0; i < numSamples; i++) {
      spec.c[i] += other.c[i];
    }
    return spec;
  }
  MySpectrum operator*(const MySpectrum& other) {
    MySpectrum spec = *this;
    for (int i = 0; i < numSamples; i++) {
      spec.c[i] *= other.c[i];
    }
    return spec;
  }

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


#endif//DATA_TYPES_H
