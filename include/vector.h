#ifndef VECTOR
#define VECTOR

struct Vector
{
  unsigned int global_size;
  unsigned int local_size;
  unsigned int first_local_entry;
  double * vals;
};

/**
 * Create a parallel vector.
 * Vector will be automatically partitioned.
 * 
 * @global_size The total size of the vector over all processors.
 */
struct Vector * createVector(unsigned int global_size);

/**
 * Print out all vector information.
 *
 * @vec The vector you want to print information about.
 */
void printVectorInformation(struct Vector * vec);

/**
 * Print out the vector information.
 *
 * @vec The vector you want to print.
 */
void printVector(struct Vector * vec);

#endif //VECTOR
