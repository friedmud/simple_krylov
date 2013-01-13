#ifndef VECTOR
#define VECTOR

struct Vector
{
  unsigned int size;
  double * vals;
};

struct Vector * createVector(unsigned int size);

#endif //VECTOR
