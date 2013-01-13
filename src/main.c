#include <stdio.h>

#include "vector.h"

int main()
{
  struct Vector * vec = createVector(10);

  printf("Size of vec: %i\n", vec->size);

  double * vals = vec->vals;

  unsigned int i=0;

  for(i=0; i<vec->size; i++)
    vals[i] = i;

  for(i=0; i<vec->size; i++)
    printf("Val: %f\n", vals[i]);
  
  return 0;
}
