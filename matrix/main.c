#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "matrix.h"


double wtime()
{
   struct timeval t;
   gettimeofday(&t, NULL);
   return t.tv_sec + t.tv_usec / 1000000.0;
}


int main(int argc, char **argv)
{
    double start_time, end_time;
    int nrows, ncols, nthreads, exec;

    matrix_t *m;
    matrix_t *A,*B;

   if ((argc != 5)) {
      printf("Uso: %s <rows> <cols> <numero de threads> <tipo de execucao>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   nrows = atoi(argv[1]);
   ncols = atoi(argv[2]);
   nthreads = atoi(argv[3]);
   exec = stoi(argv[4]);


   A = matrix_create(nrows, ncols);
   matrix_randfill(A);

   B = matrix_create(nrows, ncols);
   matrix_randfill(B);

   switch(exec){
       // SOMA
       case 0:
            matrix_t *C = matrix_sum_parallel(A, B, nthreads);

            break;

       // MULT
       case 1:
            matrix_t *C = matrix_multiply_parallel(A, B, nthreads);

            break;

       // SORT
       case 2:
            matrix_t *C = mergesort_parallel(A, B, nthreads);
            break;

   }

   start_time = wtime();


   sort_info *data = (sort_info*) malloc(sizeof(sort_info));
   data->mat = A;
   data->left  = 0;
   data->right = nrows * ncols - 1;
   data->cont = 0;
   data->nthreads = nthreads;

   // C = matrix_multiply(A,B);

   // C = matrix_sort(C);

   end_time = wtime();


   printf("\nA:\n");
   matrix_print(A);

   printf("\nB:\n");
   matrix_print(B);

   printf("\nC:\n");
   matrix_print(C);


   matrix_destroy(A);

   // matrix_destroy(B);
   matrix_destroy(C);

   //Do something




   printf("%d %d %f\n", nrows, ncols, end_time - start_time);
   fflush(stdout);


   return EXIT_SUCCESS;
}
