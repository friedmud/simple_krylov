#include <stdio.h>
#include <mpi.h>

#include "vector.h"

int main(int argc, char **argv)
{
  int rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("My rank: %i\n", rank);
  
  struct Vector * vec = createVector(11);

  printVectorInformation(vec);

  double * vals = vec->vals;

  unsigned int i = 0;

  for(i=0; i<vec->local_size; i++)
    vals[i] = vec->first_local_entry + i;

  printVector(vec);

  MPI_Finalize();
  return 0;
}
