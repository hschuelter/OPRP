// gcc -pg -Wall main.c matrix.c -o main
// ./main 200 200
// gprof main gmon.out

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include "matrix.h"

matrix_t *matrix_create(int rows, int cols){
    matrix_t *m = NULL;
    unsigned int i;

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

matrix_t *matrix_cpy(matrix_t *m){
    int i;
    matrix_t *C = matrix_create(m->rows, m->cols);

    memcpy(C->data[0], m->data[0], sizeof(double) * m->rows * m->cols);
    for(i=0; i < C->rows; i++){
        C->data[i] = &C->data[0][i*C->cols];
    }

    return C;
}

void matrix_destroy(matrix_t *m){
    free(m->data[0]);
    free(m->data);
    free(m);

    return;
}

void matrix_randfill(matrix_t *m){
    int i, j;
    for (i = 0; i < m->rows; i++) {
        for (j = 0; j < m->cols; j++) {
            m->data[i][j] = random();
        }
    }
}

void matrix_fill(matrix_t *m, double val){
   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         m->data[i][j] = val;
      }
   }
}

void matrix_print(matrix_t *m){

   int i, j;
   for (i = 0; i < m->rows; i++) {
      for (j = 0; j < m->cols; j++) {
         printf("%.2f ", m->data[i][j]);
      }
      printf("\n");
   }
   fflush(stdout);
}

void array_print(double* vet, int begin, int end){
    int i;
    for(i = begin; i <= end; i++){
        printf("%.2f ", vet[i]);
    }
    printf("\n");
}

matrix_t *matrix_sum_serial(matrix_t *A, matrix_t *B){
    //Não podemos somar
    if( (A->rows != B->rows) && (A->cols != B->cols) ){
        printf("Impossível somar\n");
        return NULL;
    }

    int i,j;
    for(i = 0; i < A->rows; i++){
        for(j = 0; j < A->cols; j++){
            A->data[i][j] += B->data[i][j];
        }
    }

    return A;
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

    long long int x = nrows * ncols / nthreads;
    for (i = 0; i < nthreads-1; i++) {
       dt[i].id = i;
       dt[i].l_i = x*i;
       dt[i].l_f = x*i + x - 1;

       dt[i].A = A;
       dt[i].B = B;
       pthread_create(&threads[i], NULL, sum_thread, (void *) (dt + i));
    }
    dt[i].id = i;
    dt[i].l_i = x*i;
    dt[i].l_f = (nrows * ncols) - 1;

    dt[i].A = A;
    dt[i].B = B;
    pthread_create(&threads[i], NULL, sum_thread, (void *) (dt + i));


    for (i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(dt);
    free(threads);

    return A;
}

void *sum_thread(void *arg){
    DadosThread *p = (DadosThread *) arg;
    int i;
    for(i = p->l_i; i <= p->l_f; i++){
        p->A->data[0][i] += p->B->data[0][i];
    }

  return NULL;
}


matrix_t *matrix_multiply_serial(matrix_t *A, matrix_t *B, matrix_t *C){
    //Não podemos multiplicar
    if(A->cols != B->rows){
        printf("Impossível multiplicar\n");
        printf("m1: %d | m2: %d\n", A->cols, B->rows);
        return NULL;
    }

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
matrix_t *matrix_multiply_parallel(matrix_t *A, matrix_t *B, matrix_t *C, int nthreads){
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
        // Ultima thread é responsável do final da última thread
        // até o fim da matriz.
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


matrix_t* matrix_sort_serial(matrix_t* m){
    int size = m->rows * m->cols;

    iterative_merge_sort(m->data[0], 0, size-1);
    return m;
}

matrix_t* matrix_sort_p(matrix_t* mat, int nthreads){
    thread_info* info = NULL;
    pthread_t *threads = NULL;

    if (!(info = (thread_info*) malloc(sizeof(thread_info) * nthreads))) {
        printf("Erro ao alocar dados da thread...\n");
        exit(EXIT_FAILURE);
    }

    if (!(threads = (pthread_t *) malloc(sizeof(pthread_t) * nthreads))) {
        printf("Erro ao alocar as threads...\n");
        exit(EXIT_FAILURE);
    }
    thread_sort_setup(info, mat, nthreads);
    int i;
    for(i = 0; i < nthreads; i++){
        pthread_create(&threads[i], NULL, sort_thread, (void*) (info + i));
    }

    for(i = 0; i < nthreads; i++){
        pthread_join(threads[i], NULL);
    }

    int size = mat->rows * mat->cols;
    double* temp1 = (double*) malloc(sizeof(double) * size);
    double* temp2 = (double*) malloc(sizeof(double) * size);
    
    for(i = 1; i < nthreads; i *= 2){
        int j;
        for(j = 0; j + i < nthreads; j += 2*i){
            merge(mat->data[0], info[j].left, info[j].right, info[j+i].right, temp1, temp2);
            info[j].right = info[j+i].right;
        }
    }
    free(info);
    free(threads);
}


void* sort_thread(void* arg){
    thread_info *data = (thread_info *) arg;

    iterative_merge_sort(data->vet, data->left, data->right);
}


void thread_sort_setup(thread_info* t_info, matrix_t* m, int nthreads){
    long long int size = m->rows * m->cols;
    long long int part = size / nthreads;
    int i;

    // Arrumando os dados para as threads
    for (i = 0; i < nthreads - 1; i++) {
       t_info[i].left = part * i;
       t_info[i].right = part * (i + 1) - 1;
       t_info[i].vet = m->data[0];
    }
    // Caso não aconteça divisão perfeita entre o tamanho da matriz 
    // e o número de threads, a última thread pega o restante.
    t_info[i].left = part * i;
    t_info[i].right = size - 1;
    t_info[i].vet = m->data[0];
}

void iterative_merge_sort(double* vet, int begin, int end){
    int current; 
    int left, mid, right;

    int size = (end - begin + 1);
    double* temp1 = (double*) malloc(sizeof(double) * size);
    double* temp2 = (double*) malloc(sizeof(double) * size);

    for (current = 1; current < size; current = 2 * current){
        for (left = begin; left < end; left += 2 * current) {
            right = min(left + 2*current - 1, end); 
            mid = left + current -1;
            
            if(mid > end) continue;

            merge(vet, left, mid, right, temp1, temp2);
        } 
    }
}

void merge(double *vet, int l, int m, int r, double* L, double* R){   
    long int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    for (i = 0; i < n1; i++)
        L[i] = vet[l + i];

    for (j = 0; j < n2; j++)
        R[j] = vet[m + j + 1];

    i = 0; j = 0; k = l;
    while (i < n1 && j < n2){
        if (L[i] <= R[j]) vet[k] = L[i++];
        else vet[k] = R[j++];
        k++;
    }

    while (i < n1) vet[k++] = L[i++];
    while (j < n2) vet[k++] = R[j++];
}

int min(int x, int y) {
    return (x < y) ? x : y;
}

void swap(double* a, double* b){
    double d = *a;
    *a = *b;
    *b = d;
}
