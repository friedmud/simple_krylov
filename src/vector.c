#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

struct Vector *
createVector(unsigned int global_size)
{
  struct Vector * vec = malloc( sizeof(struct Vector) );
  vec->global_size = global_size;

  int rank, ntasks;  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  /* Partition by number of tasks */
  unsigned int local_size = global_size / ntasks;

  vec->first_local_entry = rank * local_size;

  /* If this is the last process add any extra entries in case things aren't even */
  if(rank == ntasks - 1)
    local_size += global_size % ntasks;

  vec->local_size = local_size;
  vec->vals = malloc( local_size * sizeof(double) );


  return vec;
}

void
printVectorInformation(struct Vector * vec)
{
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("%i gs:%i ls:%i fle:%i\n", rank, vec->global_size, vec->local_size, vec->first_local_entry);
}

void printVector(struct Vector * vec)
{
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double * vals = vec->vals;
  
  unsigned int i = 0;
  for(i=0; i<vec->local_size; i++)
    printf("%i vec[%i]: %f\n", rank, vec->first_local_entry + i, vals[i]);  
}

