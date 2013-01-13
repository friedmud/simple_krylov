#ifndef VECTOR
#define VECTOR

struct Vector
{
  unsigned parallel; /* bool */
  unsigned int global_size;
  unsigned int local_size;
  unsigned int first_local_entry;
  double * vals;
};

/**
 * Create a serial vector.
 * 
 * @size The total size of the vector.
 */
struct Vector * createSerialVector(unsigned int size);

/**
 * Create a parallel vector.
 * Vector will be automatically partitioned.
 * 
 * @global_size The total size of the vector over all processors.
 */
struct Vector * createParallelVector(unsigned int global_size);

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

/**
 * Vector 2 Norm
 *
 * @return The 2 Norm of the vector
 */
double vector2Norm(struct Vector * vec);

/**
 * Vector Dot Product
 *
 * @return The dot product of a and b 
 */
double vectorDot(struct Vector * a, struct Vector * b);

#endif //VECTOR
