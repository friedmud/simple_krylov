#include "test_vector.h"
#include "vector.h"

#include <assert.h>
#include <stdio.h>
#include <mpi.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>

void testVector()
{
  struct Vector * vec = createParallelVector(10);
  assert(vec->global_size == 10);
  
  struct Vector * vec2 = createSerialVector(10);

  {
    double * vals = vec->vals;
    unsigned int i = 0;
    for(i=0; i<vec->local_size; i++)
      vals[i] = 2;

    for(i=0; i<vec->local_size; i++)
      assert(vals[i] == 2);
  }

  {
    double * vals = vec2->vals;
    unsigned int i = 0;
    for(i=0; i<vec2->local_size; i++)
      vals[i] = 2;
  }  

  printVectorInformation(vec);
  printVector(vec);

  {
    /*
    struct timeb t1,t2;    
    ftime(&t1);
    */
    double dot = vectorDot(vec, vec2);

    /*
    ftime(&t2);
    printf("Dot Time: %i\n", (int) (t2.millitm - t1.millitm));
    */

    assert(dot == 40); 
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("%i vectorDot: %f\n", rank, dot);
  }

  destroyVector(vec);
  destroyVector(vec2);
}
