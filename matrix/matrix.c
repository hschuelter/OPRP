// gcc -pg -Wall main.c matrix.c -o main
// ./main 200 200
// gprof main gmon.out

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include "matrix.h"

matrix_t *matrix_create(int rows, int cols)
{
    matrix_t *m = NULL;
    int i;

    m = (matrix_t*) malloc( sizeof(matrix_t) );

    m->rows = rows;
    m->cols = cols;

    m->data = (double**) malloc(sizeof(double*) * rows);
    double* mat = (double*) malloc(sizeof(double) * rows * cols);

    for(i=0; i < rows; i++){
        m->data[i] = &mat[i*cols];
    }

    matrix_fill(m, 0.0);

    return m;
}

void matrix_destroy(matrix_t *m)
{
    free(m->data[0]);
    free(m->data);
    free(m);

    return;
}

void matrix_randfill(matrix_t *m)
{
   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         m->data[i][j] = random();
      }
   }
}

void matrix_fill(matrix_t *m, double val)
{
   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         m->data[i][j] = val;
      }
   }
}

matrix_t *matrix_multiply(matrix_t *A, matrix_t *B)
{
    //Não podemos multiplicar
    if(A->cols != B->rows){
        printf("Impossível multiplicar\n");
        return NULL;
    }

    matrix_t *C = matrix_create(A->rows, B->cols);
    // matrix_fill(C, 0.0);

    int i,j,k;

    for(i = 0; i < A->rows; i++){
        for(j = 0; j < B->cols; j++){
            for(k = 0; k < A->cols; k++){
                C->data[i][j] += A->data[i][k] * B->data[k][j];
            }
        }
    }

    return C;
}


//Cada thread é responsável por um determinado número de linhas da matriz C
matrix_t *matrix_multiply_parallel(matrix_t *A, matrix_t *B, int nthreads){
   if(A->cols != B->rows){
        printf("Impossível multiplicar\n");
        exit(EXIT_FAILURE);
    }


    DadosThread *dt = NULL;
    pthread_t *threads = NULL;

    if (!(dt = (DadosThread *) malloc(sizeof(DadosThread) * nthreads))) {
       printf("Erro ao alocar dados da thread...\n");
       exit(EXIT_FAILURE);
    }

    if (!(threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads))) {
        printf("Erro ao alocar as threads...\n");
               exit(EXIT_FAILURE);
    }

    int nrows = A->rows;
    int ncols = B->cols;
    int i;
    long long int x;

    matrix_t *C = matrix_create(nrows, ncols);
    
    if(nthreads > nrows){ //Não é possível dividir entre todas a threads
        nthreads = nrows;
        for (i = 0; i < nthreads; i++) {
            dt[i].id = i;
            dt[i].l_i = i;
            dt[i].l_f = i;

            dt[i].A = A;
            dt[i].B = B;
            dt[i].C = C;
            pthread_create(&threads[i], NULL, multiply_thread, (void *) (dt + i));
        }
    }
    else{
        x = nrows / nthreads;
        for (i = 0; i < nthreads-1; i++) {
            dt[i].id = i;
            dt[i].l_i = x*i;
            dt[i].l_f = x*i + x - 1;

            dt[i].A = A;
            dt[i].B = B;
            dt[i].C = C;
            pthread_create(&threads[i], NULL, multiply_thread, (void *) (dt + i));
        }
        dt[i].id = i;
        dt[i].l_i = x*i;
        dt[i].l_f = nrows - 1;

        dt[i].A = A;
        dt[i].B = B;
        dt[i].C = C;
        pthread_create(&threads[i], NULL, multiply_thread, (void *) (dt + i));
    }

    for (i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dt);
    free(threads);

    return C;
}

void *multiply_thread(void *arg){
    DadosThread *p = (DadosThread *) arg;

  // printf("Thread %d: %d %d\n", p->id, p->l_i, p->l_f);
    
    int i, j, k;
    for(i = p->l_i; i <= p->l_f; i++){ //Percorre as linhas de C relevantes
        for(j = 0; j < p->C->cols; j++){ //Percorre cada coluna de C
            for(k = 0; k < p->A->cols; k++){ 
                p->C->data[i][j] += p->A->data[i][k]*p->B->data[k][j];
            }
        }
    }


    return NULL;    
}


void matrix_print(matrix_t *m)
{

   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         printf("%.2f ", m->data[i][j]);
      }
      printf("\n");
   }
   fflush(stdout);
}

matrix_t *matrix_sum(matrix_t *A, matrix_t *B)
{
    //Não podemos somar
    if( (A->rows != B->rows) && (A->cols != B->cols) ){
        printf("Impossível somar\n");
        return NULL;
    }

    matrix_t *C = matrix_create(A->rows, A->cols);

    int i,j;

    for(i = 0; i < A->rows; i++){
        for(j = 0; j < A->cols; j++){
            C->data[i][j] = A->data[i][j] + B->data[i][j];
        }
    }

    return C;
}

matrix_t *matrix_sum_parallel(matrix_t *A, matrix_t *B, int nthreads){
    DadosThread *dt = NULL;
    pthread_t *threads = NULL;

    if (!(dt = (DadosThread *) malloc(sizeof(DadosThread) * nthreads))) {
       printf("Erro ao alocar dados da thread...\n");
       exit(EXIT_FAILURE);
    }

    if (!(threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads))) {
        printf("Erro ao alocar as threads...\n");
               exit(EXIT_FAILURE);
    }

    int nrows = A->rows;
    int ncols = A->cols;
    int i;

    matrix_t *C = matrix_create(nrows, ncols);
    
    long long int x = nrows * ncols / nthreads;
    for (i = 0; i < nthreads-1; i++) {
       dt[i].id = i;
       dt[i].l_i = x*i;
       dt[i].l_f = x*i + x - 1;

       dt[i].A = A;
       dt[i].B = B;
       dt[i].C = C;
       pthread_create(&threads[i], NULL, sum_thread, (void *) (dt + i));
    }
    dt[i].id = i;
    dt[i].l_i = x*i;
    dt[i].l_f = (nrows * ncols) - 1;

    dt[i].A = A;
    dt[i].B = B;
    dt[i].C = C;
    pthread_create(&threads[i], NULL, sum_thread, (void *) (dt + i));


    for (i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dt);
    free(threads);

    return C;
}

void *sum_thread(void *arg){
    DadosThread *p = (DadosThread *) arg;
    int i;
    for(i = p->l_i; i <= p->l_f; i++){
        p->C->data[0][i] = p->A->data[0][i] + p->B->data[0][i];
    }

  // printf("Thread %d terminou\n", p->id);

  return NULL;
}


matrix_t *matrix_cpy(matrix_t *A){
    int i;
    matrix_t *C = matrix_create(A->rows, A->cols);

    memcpy(C->data[0], A->data[0], sizeof(double) * A->rows * A->cols);
    for(i=0; i < C->rows; i++){
        C->data[i] = &C->data[0][i*C->cols];
    }

    return C;
}

matrix_t *matrix_sort(matrix_t *A)
{
    int i,j;
    double valor;
    matrix_t *C = matrix_cpy(A);

    for(i = 0; i < A->rows * A->cols; i++){
        for(j = i; j < A->rows * A->cols; j++){
            if(C->data[0][j] < C->data[0][i]){
                valor = C->data[0][i];
                C->data[0][i] = C->data[0][j];
                C->data[0][j] = valor;
            }
        }
    }

    return C;
}
