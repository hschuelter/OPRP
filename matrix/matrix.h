#ifndef __MATRIX_H
#define __MATRIX_H

#include <stdlib.h>
#include <string.h>

#define random() ((rand() ^ rand()) / (RAND_MAX + 1.0))

typedef struct {
      double **data;
      int rows;
      int cols;
} matrix_t;

typedef struct {
	int id;
	matrix_t *A;
	matrix_t *B;
	matrix_t *C;

    double *vet;
    int vet_size;

    int nthreads;
    pthread_t *threads;

    int** manip_atual;

	int l_i;
	int l_f;
} DadosThread;

/*
 * All functions must return a new matriz (when needed)
 */


matrix_t *matrix_create(int rows, int cols);

void matrix_destroy(matrix_t *m);

void matrix_randfill(matrix_t *m);

void matrix_fill(matrix_t *m, double val);

matrix_t *matrix_multiply_serial(matrix_t *A, matrix_t *B);
matrix_t *matrix_multiply_parallel(matrix_t *A, matrix_t *B, int nthreads);
void *multiply_thread(void *arg);

matrix_t *matrix_sum_serial(matrix_t *A, matrix_t *B);
matrix_t *matrix_sum_parallel(matrix_t *A, matrix_t *B, int nthreads);
void *sum_thread(void *arg);

matrix_t *matrix_sort_serial(matrix_t *A);

matrix_t *mergesort_parallel(matrix_t* A, int nthreads);
void *mergesort_thread(void *arg);
matrix_t *coisa_merge(DadosThread *dt, int nthreads, int rows, int cols);

void *mergesort(void *data);


matrix_t* matrix_sort_serial(matrix_t* mat);

void mergeSort(matrix_t *mat, int l, int r);
void merge(double *vet, int l, int m, int r);

void matrix_print(matrix_t *m);

int last_two_multiple(int n);

#endif
