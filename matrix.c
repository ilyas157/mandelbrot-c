#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"

Matrix *matrix_create(int rows, int cols)
{
    if (rows > 0 && cols > 0)
    {
        double *data = malloc(sizeof(double) * rows * cols);
        Matrix *matrix = malloc(sizeof(Matrix));
        matrix->cols = cols;
        matrix->rows = rows;
        matrix->data = data;
        return matrix;
    }
    else
    {
        return NULL;
    }
}
Matrix *matrix_random(int rows, int cols)
{

    Matrix *m = matrix_create(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrix_set(m, i, j, ((double)rand() / RAND_MAX) * 20.0 - 10.0);
        }
    }
    return m;
}

void matrix_free(Matrix *matrix)
{
    free(matrix->data);
    free(matrix);
}

void matrix_set(Matrix *m, int i, int j, double value)
{
    m->data[i * m->cols + j] = value;
}

double matrix_get(Matrix *m, int i, int j)
{
    return m->data[i * m->cols + j];
}

Matrix *matrix_add(Matrix *A, Matrix *B)
{
    if (A->cols == B->cols && A->rows == B->rows)
    {
        int size = A->cols * A->rows;
        Matrix *C = matrix_create(A->rows, A->cols);
        for (int i = 0; i < size; i++)
        {
            C->data[i] = A->data[i] + B->data[i];
        }
        return C;
    }
    else
    {
        return NULL;
    }
}

Matrix *matrix_subtract(Matrix *A, Matrix *B)
{
    if (A->cols == B->cols && A->rows == B->rows)
    {
        int size = A->cols * A->rows;
        Matrix *C = matrix_create(A->rows, A->cols);
        for (int i = 0; i < size; i++)
        {
            C->data[i] = A->data[i] - B->data[i];
        }
        return C;
    }
    else
    {
        return NULL;
    }
}

void matrix_print(Matrix *m)
{
    int cols = m->cols;
    int rows = m->rows;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%.2lf ", matrix_get(m, i, j));
        }
        printf("\n");
    }
    printf("\n");
}

Matrix *matrix_multiply(Matrix *A, Matrix *B)
{
    
    if (A->cols == B->rows)
    {
        int rows = A->rows;
        int cols = B->cols;
        double value = 0;
        Matrix *C = matrix_create(rows, cols);
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                for (int k = 0; k < A->cols; k++)
                {
                    value += matrix_get(A, i, k) * matrix_get(B, k, j);
                }
                matrix_set(C, i, j, value);
                value = 0;
            }
            printf("\rProgress:  %.2f%%", (i+1) *100.0 / rows);
            fflush(stdout);
        }
        printf("\n");
        return C;
    }
    else
    {
        return NULL;
    }
}

void matrix_transpose(Matrix **m)
{

    Matrix *temp = matrix_create((*m)->cols, (*m)->rows);
    for (int i = 0; i < temp->rows; i++)
    {
        for (int j = 0; j < temp->cols; j++)
        {
            matrix_set(temp, i, j, matrix_get(*m, j, i));
        }
    }
    matrix_free(*m);
    *m = temp;
}



Matrix *matrix_multiplyOptimized(Matrix *A, Matrix *B)
{
    
    if (A->cols == B->rows)
    {
        int rows = A->rows;
        int cols = B->cols;
        
        matrix_transpose(&B);
        Matrix *C = matrix_create(rows, cols);

        //#pragma omp parallel for
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                double value = 0;
                for (int k = 0; k < A->cols; k++)
                {
                    value += matrix_get(A, i, k) * matrix_get(B, j, k);
                }
                matrix_set(C, i, j, value);
                value = 0;
            }
        }
        return C;
    }
    else
    {
        return NULL;
    }
}