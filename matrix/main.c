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
    int nrows, ncols, nthreads;

    matrix_t *m;
    matrix_t *A,*B;

   if ((argc != 4)) {
      printf("Uso: %s <rows> <cols> <numero de threads>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   nrows = atoi(argv[1]);
   ncols = atoi(argv[2]);
   nthreads = atoi(argv[3]);


   A = matrix_create(nrows, ncols);
   matrix_randfill(A);

   B = matrix_create(nrows, ncols);
   matrix_randfill(B);

   start_time = wtime();




   matrix_t *C = mergesort_parallel(A, nthreads);

   // C = matrix_multiply(A,B);

   // C = matrix_sort(C);

   end_time = wtime();


   printf("\nA:\n");
   matrix_print(A);

   // printf("\nB:\n");
   // matrix_print(B);

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
