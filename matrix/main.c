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


int main(int argc, char **argv){
    double start_time, end_time;
    int nrows, ncols, nthreads, exec;

    matrix_t *m1, *m2;
    matrix_t *A,*B;

   if ((argc != 5)) {
      printf("Uso: %s <rows> <cols> <numero de threads> <tipo de execucao>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

    nrows = atoi(argv[1]);
    ncols = atoi(argv[2]);
    nthreads = atoi(argv[3]);
    exec = atoi(argv[4]);

    m1 = matrix_create(nrows, ncols);
    matrix_randfill(m1);

    // printf("\nm1:\n");
    // matrix_print(m1);

    start_time = wtime();
    m1 = matrix_sort_parallel(m1, nthreads);

    end_time = wtime();

    // printf("\nm1:\n");
    // matrix_print(m1);
    
    matrix_destroy(m1);
    matrix_destroy(m2);

    printf("\n%d %d %f\n", nrows, ncols, end_time - start_time);
    fflush(stdout);

    return EXIT_SUCCESS;
}
