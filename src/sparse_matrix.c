#include "sparse_matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <assert.h>

struct SparseMatrix *
createParallelSparseMatrix(unsigned int global_rows, unsigned int global_cols, unsigned int * global_nonzero_entries_per_row, unsigned int ** global_nonzero_entries)
{
  struct SparseMatrix * mat = malloc( sizeof(struct SparseMatrix) );

  mat->parallel = 1;
  mat->global_rows = global_rows;
  mat->global_cols = global_cols;

  int rank, ntasks;  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

  /* Partition rows by number of tasks */
  unsigned int local_rows = global_rows / ntasks;

  unsigned int first_local_row = rank * local_rows;
  mat->first_local_row = first_local_row;
  mat->last_local_row = first_local_row + local_rows;

  /* If this is the last process add any extra rows in case things aren't even */
  if(rank == ntasks - 1)
    local_rows += global_rows % ntasks;

  mat->local_rows = local_rows;

  mat->nonzero_entries_per_row = malloc( local_rows * sizeof(unsigned int) );

  unsigned int i=0;
  for(i=0; i<local_rows; i++)
    mat->nonzero_entries_per_row[i] = global_nonzero_entries_per_row[first_local_row + i];

  mat->nonzero_entries = malloc( local_rows * sizeof(unsigned int *) );

  for(i=0; i<local_rows; i++)
  {
    unsigned int nonzeros = mat->nonzero_entries_per_row[i];
    mat->nonzero_entries[i] = malloc( nonzeros * sizeof(unsigned int) );

    unsigned int j=0;
    for(j=0; j<nonzeros; j++)
      mat->nonzero_entries[i][j] = global_nonzero_entries[first_local_row + i][j];
  }

  mat->vals = malloc( local_rows * sizeof(double *) );
  
  for(i=0; i<local_rows; i++)
    mat->vals[i] = malloc( mat->nonzero_entries_per_row[i] * sizeof(double) );    

  return mat;
}

void destroySparseMatrix(struct SparseMatrix * mat)
{
  unsigned int i=0;
  for(i=0; i<mat->local_rows; i++)
  {
    free(mat->nonzero_entries[i]);
    free(mat->vals[i]);
  }

  free(mat->nonzero_entries_per_row);
  free(mat->nonzero_entries);
  free(mat->vals);

  free(mat);
}

void
printSparseMatrix(struct SparseMatrix * mat)
{
  unsigned int local_rows = mat->local_rows;
  
  unsigned int i=0;
  unsigned int j=0;

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  for(i=0; i<local_rows; i++)
    for(j=0; j<mat->nonzero_entries_per_row[i]; j++)
      printf("%i  %i %i: %f\n", rank, mat->first_local_row + i, mat->nonzero_entries[i][j], mat->vals[i][j]);
}

double
getGlobalSparseMatrixEntry(struct SparseMatrix * mat, unsigned int global_row, unsigned int global_col)
{
  assert(global_row >= mat->first_local_row && global_row < mat->last_local_row);

  unsigned int local_row = global_row - mat->first_local_row;

  /* Now search to find the local column associated with this global index */
  unsigned int * nonzero_entries_on_row = mat->nonzero_entries[local_row];

  unsigned int found_it = 0;
  unsigned int local_col = 0;

  unsigned int j = 0;
  for(j = 0; j < mat->nonzero_entries_per_row[local_row]; j++)
  {
    if(nonzero_entries_on_row[j] == global_col)
    {
      found_it = 1;
      local_col = j;
      break;
    }
  }

  assert(found_it);

  return mat->vals[local_row][local_col];
}

void
setGlobalSparseMatrixEntry(struct SparseMatrix * mat, unsigned int global_row, unsigned int global_col, double value)
{
  assert(global_row >= mat->first_local_row && global_row < mat->last_local_row);

  unsigned int local_row = global_row - mat->first_local_row;

  /* Now search to find the local column associated with this global index */
  unsigned int * nonzero_entries_on_row = mat->nonzero_entries[local_row];

  unsigned int found_it = 0;
  unsigned int local_col = 0;

  unsigned int j = 0;
  for(j = 0; j < mat->nonzero_entries_per_row[local_row]; j++)
  {
    if(nonzero_entries_on_row[j] == global_col)
    {
      found_it = 1;
      local_col = j;
      break;
    }
  }

  assert(found_it);

  mat->vals[local_row][local_col] = value;
}

  
