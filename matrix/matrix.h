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

typedef struct {
    int id;
    double* vet;
    int left;
    int right;

} thread_info;

/*
 * All functions must return a new matriz (when needed)
 */

matrix_t *matrix_create(int rows, int cols);
matrix_t *matrix_cpy(matrix_t *m);
void matrix_destroy(matrix_t *m);

void matrix_randfill(matrix_t *m);
void matrix_fill(matrix_t *m, double val);

void matrix_print(matrix_t *m);
void array_print(double* vet, int begin, int end);

matrix_t *matrix_sum_serial(matrix_t *A, matrix_t *B);
matrix_t *matrix_sum_parallel(matrix_t *A, matrix_t *B, int nthreads);
void *sum_thread(void *arg);

matrix_t *matrix_multiply_serial(matrix_t *A, matrix_t *B, matrix_t *C);
matrix_t *matrix_multiply_parallel(matrix_t *A, matrix_t *B, matrix_t *C, int nthreads);
void *multiply_thread(void *arg);

/*======================================*/

matrix_t* matrix_sort_serial(matrix_t* m);
matrix_t* matrix_sort_p(matrix_t* mat, int nthreads);
void* sort_thread(void* arg); 
void thread_sort_setup(thread_info* t_info, matrix_t* m, int nthreads);

void recursive_merge_sort(matrix_t *mat, int l, int r);
void iterative_merge_sort(double* vet, int begin, int end);
void merge(double *vet, int l, int m, int r, double* L, double* R);

int min(int x, int y);
void swap(double* a, double* b);

#endif
