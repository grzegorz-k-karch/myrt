#ifndef DATA_TYPES_H
#define DATA_TYPES_H

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


#endif//DATA_TYPES_H
