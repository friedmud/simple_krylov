#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <assert.h>

#include "test_vector.h"
#include "sparse_matrix.h"

int main(int argc, char **argv)
{
  int rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("My rank: %i\n", rank);

/*  testVector(); */

  unsigned int * global_nonzero_entries_per_row = malloc( 2 * sizeof(unsigned int) );

  global_nonzero_entries_per_row[0] = 1;
  global_nonzero_entries_per_row[1] = 1;
  
  unsigned int ** global_nonzero_entries = malloc( 2 * sizeof(unsigned int *) );

  global_nonzero_entries[0] = malloc( 1 * sizeof(unsigned int) );
  global_nonzero_entries[1] = malloc( 1 * sizeof(unsigned int) );

  global_nonzero_entries[0][0] = 0;
  global_nonzero_entries[1][0] = 1;

  struct SparseMatrix * mat = createParallelSparseMatrix(2, 2, global_nonzero_entries_per_row, global_nonzero_entries);

  printf("%i First local row: %i\n", rank, mat->first_local_row);

  setGlobalSparseMatrixEntry(mat, 0, 0, 2);
  setGlobalSparseMatrixEntry(mat, 1, 1, 3);

  printSparseMatrix(mat);

  struct SparseMatrix * mat2 = readCRSSparseMatrix("dummy.crs");

  printSparseMatrix(mat2);

  MPI_Finalize();
  return 0;
}
