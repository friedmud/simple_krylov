#include "vector.h"

#include <stdlib.h>

struct Vector *
createVector(unsigned int size)
{
  struct Vector * vec = malloc( sizeof(struct Vector) );
  vec->size = size;
  vec->vals = malloc( size * sizeof(double) );

  return vec;
}
