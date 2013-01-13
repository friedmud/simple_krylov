#include <stdio.h>
#include <mpi.h>

#include "test_vector.h"

int main(int argc, char **argv)
{
  int rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("My rank: %i\n", rank);

  testVector();

  MPI_Finalize();
  return 0;
}
