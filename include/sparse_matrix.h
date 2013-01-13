#ifndef SPARSEMATRIX
#define SPARSEMATRIX

/**
 * Matrix in CRS format.
 */
struct SparseMatrix
{
  unsigned int parallel;
  unsigned int global_rows, global_cols;
  unsigned int local_rows;
  
  unsigned int first_local_row;

  /**
   * One _beyond_ the actual last local entry
   */
  unsigned int last_local_row;

  /**
   * Number of entries on each row.
   */
  unsigned int * nonzero_entries_per_row;

  /**
   * Which entries on a row are actually nonzero
   */
  unsigned int ** nonzero_entries;

  /**
   * Actual entries in each row
   */
  double ** vals;
};

/**
 * Create a Parallel SparseMatrix
 */
struct SparseMatrix * createParallelSparseMatrix(const unsigned int global_rows, const  unsigned int global_cols, const unsigned int * global_nonzero_entries_per_row, unsigned int ** global_nonzero_entries);

/**
 * Destroy a SparseMatrix.
 */
void destroySparseMatrix(struct SparseMatrix * mat);

/**
 * Print out the entries of a SparseMatrix
 */
void printSparseMatrix(const struct SparseMatrix * mat);

/**
 * Read a matrix from a file in CRS format
 * 
 * Format is as follows:
 * global_rows global_cols
 * num_nonzero_entries col_nums vals
 */
struct SparseMatrix * readCRSSparseMatrix(char * file_name);

/**
 * Get an entry in the matrix given global rows and columns.
 * Note that this is _slow_!
 *
 * Also note that you cannot call this with a global_row that is not on the current processor!
 */
double getGlobalSparseMatrixEntry(const struct SparseMatrix * mat, const unsigned int global_row, const unsigned int global_col);

/**
 * Set an entry in the matrix given global rows and columns.
 * Note that this is _slow_!
 * 
 * Also note that you cannot call this with a global_row that is not on the current processor!
 */
void setGlobalSparseMatrixEntry(struct SparseMatrix * mat, const unsigned int global_row, const unsigned int global_col, const double value);


#endif //SPARSEMATRIX
