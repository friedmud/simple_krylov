#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <assert.h>

struct Vector *
createSerialVector(unsigned int size)
{
  struct Vector * vec = malloc( sizeof(struct Vector) );

  vec->parallel = 0;
  vec->global_size = size;
  vec->local_size = size;
  vec->first_local_entry = 0;
  vec->vals = malloc( size * sizeof(double) );
  
  return vec;
}

struct Vector *
createParallelVector(unsigned int global_size)
{
  struct Vector * vec = malloc( sizeof(struct Vector) );

  vec->parallel = 1;
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

double vector2Norm(struct Vector * vec)
{
  double * vals = vec->vals;
  
  double local_norm = 0;

  unsigned int i = 0;
  for(i=0; i<vec->local_size; i++)
    local_norm += vals[i] * vals[i];

  double global_norm = 0;
  MPI_Allreduce(&local_norm, &global_norm, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  global_norm = sqrt(global_norm);

  return global_norm;
}

double vectorDot(struct Vector * a, struct Vector * b)
{
  assert(a->global_size == b->global_size);
  
  double * a_vals = a->vals;
  double * b_vals = b->vals;
  
  double local_dot = 0;

  /* If they are both parallel then we can just multiply them */
  if(a->parallel && b->parallel)
  {
    int local_size = a->local_size;
    
    unsigned int i = 0;
    for(i=0; i<local_size; i++)
      local_dot += a_vals[i] * b_vals[i];
  }
  else if(a->parallel) /* b is parallel */
  {
    int local_size = a->local_size;
    int first_local_entry = a->first_local_entry;
    
    unsigned int i = 0;
    for(i=0; i<local_size; i++)
      local_dot += a_vals[i] * b_vals[first_local_entry + i];
  }
  else /* a is parallel */
  {
    int local_size = b->local_size;
    int first_local_entry = b->first_local_entry;
    
    unsigned int i = 0;
    for(i=0; i<local_size; i++)
      local_dot += a_vals[first_local_entry + i] * b_vals[i];
  }

  double global_dot = 0;
  MPI_Allreduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

  return global_dot;
}
