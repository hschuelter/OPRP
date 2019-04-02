#include <stdio.h>
#include <omp.h>

int main(int argc, char* argv[]){
    int tid, i; 
    float sum;
    float a[1000], b[1000];

    // #pragma omp parallel num_threads (8) private(tid)
    // {
    //     tid = omp_get_thread_num();
    //     printf("Asteristico: %d\n", tid);
    // }


    #pragma omp parallel for num_threads(8)
    for(i = 0; i < 1000; i++)
        a[i] = b[i] = i * 1.0;

    sum = 0.0;

    #pragma omp parallel for num_threads(8) reduction(+: sum) 
    for(i = 0; i < 1000; i++)
        // printf("a[%d] = %d\n", i, a[i]);
        sum = sum + (a[i]*b[i]);
    
    printf("%f\n", sum);

    return 0;
}