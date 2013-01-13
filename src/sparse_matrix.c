#include "sparse_matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <assert.h>
#include <string.h>

struct SparseMatrix *
createParallelSparseMatrix(const unsigned int global_rows, const unsigned int global_cols, const unsigned int * global_nonzero_entries_per_row, unsigned int ** global_nonzero_entries)
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

  /* If this is the last process add any extra rows in case things aren't even */
  if(rank == ntasks - 1)
    local_rows += global_rows % ntasks;

  mat->local_rows = local_rows;

  mat->last_local_row = first_local_row + local_rows;

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
printSparseMatrix(const struct SparseMatrix * mat)
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

struct SparseMatrix *
readCRSSparseMatrix(char * file_name)
{
  struct SparseMatrix * mat = malloc( sizeof(struct SparseMatrix));
  
  char line[100000];

  FILE * fr = fopen(file_name, "rt");

  unsigned int global_rows, global_cols;

  /* Read header containing number of rows and columns */
  if(fgets(line, 100000, fr))
  {
    char * rows_tok = strtok(line, " ");
    assert(rows_tok);
    char * cols_tok = strtok(NULL, " ");
    assert(cols_tok);

    sscanf(rows_tok, "%u", &global_rows);
    sscanf(cols_tok, "%u", &global_cols);
  }
  else
  {
    printf("Nothing in file!");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  assert(global_rows);
  assert(global_cols);

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

  /* If this is the last process add any extra rows in case things aren't even */
  if(rank == ntasks - 1)
    local_rows += global_rows % ntasks;

  mat->local_rows = local_rows;

  mat->last_local_row = first_local_row + local_rows;

  mat->nonzero_entries_per_row = malloc( local_rows * sizeof(unsigned int) );
  mat->nonzero_entries = malloc( local_rows * sizeof(unsigned int *) );
  mat->vals = malloc( local_rows * sizeof(double *) );

  unsigned int i=0;

  /* Chew up rows until we get to the section that is going to be on this processor */
  for(i=0; i<first_local_row; i++)
  {
    if(fgets(line, 100000, fr) == NULL)
      break;
  }

  /* Read in the rows we're going to store on this processor */
  for(i=0; i<local_rows; i++)
  {
    if(fgets(line, 100000, fr) == NULL)
      break;

    /* Grab the number of nonzeros on this row */
    char * num_entries_tok = strtok(line, " ");
    assert(num_entries_tok);

    unsigned int num_entries;
    sscanf(num_entries_tok, "%u", &num_entries);

    mat->nonzero_entries_per_row[i] = num_entries;
    mat->nonzero_entries[i] = malloc( num_entries * sizeof(unsigned int) );
    mat->vals[i] = malloc( num_entries * sizeof(double) );
    
    /* Read out the column numbers */
    unsigned int j=0;
    for(j=0; j<num_entries; j++)
    {
      char * col_tok = strtok(NULL, " ");
      assert(col_tok);

      unsigned int col;
      sscanf(col_tok, "%u", &col);

      mat->nonzero_entries[i][j] = col;
    }

    /* Read out the values */
    for(j=0; j<num_entries; j++)
    {
      char * val_tok = strtok(NULL, " ");
      assert(val_tok);

      double val;
      sscanf(val_tok, "%lf", &val);

      mat->vals[i][j] = val;
    }
  }
  
  fclose(fr);

  return mat;
}

double
getGlobalSparseMatrixEntry(const struct SparseMatrix * mat, const unsigned int global_row, const unsigned int global_col)
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
setGlobalSparseMatrixEntry(struct SparseMatrix * mat, const unsigned int global_row, const unsigned int global_col, const double value)
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

  
