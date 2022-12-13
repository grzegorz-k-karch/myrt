#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <vector_types.h>

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

template <int numSamples>
class MySpectrum {

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
    return this;
  }
  MySpectrum operator*=(const MySpectrum& other) {
    for (int i = 0; i < numSamples; i++) {
      c[i] *= other.c[i];
    }
    return this;
  }
  
private:
  Float c[numSamples];
};

static const int numSpectralSamples = 60;

typedef MySpectrum<numSpectralSamples> Spectrum;


#endif//DATA_TYPES_H
