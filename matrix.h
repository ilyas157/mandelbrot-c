#ifndef MATRIX
#define MATRIX

typedef struct
{
    double *data;
    int rows;
    int cols;
} Matrix;

Matrix *matrix_create(int rows, int cols);
void matrix_free(Matrix *matrix);
void matrix_set(Matrix *m, int i, int j, double value);
double matrix_get(Matrix *m, int i, int j);
Matrix *matrix_add(Matrix *A, Matrix *B);
Matrix *matrix_subtract(Matrix *A, Matrix *B);
void matrix_print(Matrix *m);
Matrix *matrix_multiply(Matrix *A, Matrix *B);
Matrix *matrix_multiplyOptimized(Matrix *A, Matrix *B);
void matrix_transpose(Matrix **m);
Matrix *matrix_random(int rows, int cols);
#endif