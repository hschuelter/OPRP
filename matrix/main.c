#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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
    int nrows, ncols;

    matrix_t *m;
    matrix_t *A,*B,*C;

   if ((argc != 3)) {
      printf("Uso: %s <rows> <cols>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   nrows = atoi(argv[1]);
   ncols = atoi(argv[2]);

   start_time = wtime();


   A = matrix_create(nrows, ncols);
   matrix_randfill(A);
   //printf("A:\n");
   // matrix_print(A);


   B = matrix_create(nrows, ncols);
   matrix_randfill(B);
   // printf("B:\n");
   // matrix_print(B);


   C = matrix_sum(A,B);
   C = matrix_multiply(A,B);
   // printf("C:\n");
   // matrix_print(C);

   C = matrix_sort(C);
   // printf("C:\n");
   // matrix_print(C);


   // m = matrix_create(nrows, ncols);
   // matrix_print(m);
   // matrix_destroy(m);

   //Do something

   end_time = wtime();

   printf("%d %d %f\n", nrows, ncols, end_time - start_time);
   fflush(stdout);

   return EXIT_SUCCESS;
}
