#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "matrix.h"

void *trabalho_da_thread(void *arg);

double wtime()
{
   struct timeval t;
   gettimeofday(&t, NULL);
   return t.tv_sec + t.tv_usec / 1000000.0;
}

typedef struct {
	int id;
	matrix_t *A;
	matrix_t *B;
	matrix_t *C;
	int l_i;
	int l_f;
} DadosThread;


int main(int argc, char **argv)
{
    double start_time, end_time;
    int nrows, ncols, nthreads;
    int i;

    matrix_t *m;
    matrix_t *A,*B,*C;

   if ((argc != 4)) {
      printf("Uso: %s <rows> <cols> <numero de threads>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   nrows = atoi(argv[1]);
   ncols = atoi(argv[2]);
   nthreads = atoi(argv[3]);

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

   A = matrix_create(nrows, ncols);
   matrix_randfill(A);

   B = matrix_create(nrows, ncols);
   matrix_randfill(B);

   C = matrix_create(nrows, ncols);
   matrix_randfill(C);


   long long int x = nrows * ncols / nthreads;
   for (i = 0; i < nthreads-1; i++) {
       dt[i].id = i;
       dt[i].l_i = x*i;
       dt[i].l_f = x*i + x - 1;

       dt[i].A = A;
       dt[i].B = B;
       dt[i].C = C;
       pthread_create(&threads[i], NULL, trabalho_da_thread, (void *) (dt + i));
   }
   dt[i].id = i;
   dt[i].l_i = x*i;
   dt[i].l_f = (nrows * ncols) - 1;

   dt[i].A = A;
   dt[i].B = B;
   dt[i].C = C;
   pthread_create(&threads[i], NULL, trabalho_da_thread, (void *) (dt + i));



   start_time = wtime();



   // C = matrix_multiply(A,B);

   // C = matrix_sort(C);


   // m = matrix_create(nrows, ncols);
   // matrix_destroy(m);

   //Do something

   for (i = 0; i < nthreads; i++) {
       pthread_join(threads[i], NULL);
   }

   // printf("A:\n");
   // matrix_print(A);
   //
   // printf("B:\n");
   // matrix_print(B);
   //
   // printf("C:\n");
   // matrix_print(C);

   end_time = wtime();

   printf("%d %d %f\n", nrows, ncols, end_time - start_time);
   fflush(stdout);


   free(dt);
   free(threads);

   return EXIT_SUCCESS;
}

void *trabalho_da_thread(void *arg)
{
	DadosThread *p = (DadosThread *) arg;
    int i;
    for(i = p->l_i; i <= p->l_f; i++){
        p->C->data[0][i] = p->A->data[0][i] + p->B->data[0][i];
    }

	// printf("Thread %d terminou\n", p->id);

	return NULL;
}
