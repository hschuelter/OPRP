// nvcc sum_array.cu -o sum_array.out
#include <stdio.h>
#define N 10

__global__ void vecAdd(int* a, int* b, int* c);
void print_array(int* a);

int main(void){
    int *a, *b, *c;
    int *dev_a, *dev_b, *dev_c;

    a = (int*) malloc(sizeof(int) * N);
    b = (int*) malloc(sizeof(int) * N);
    c = (int*) malloc(sizeof(int) * N);

    for(int i = 0; i < N; i++){
        a[i] = i;
        b[i] = N - i - 1;
    }
    printf("a -> "); print_array(a);
    printf("b -> "); print_array(b);
    printf("c -> "); print_array(c);

    int size = N * sizeof(int);

    cudaMalloc( (void**) &dev_a, size);
    cudaMalloc( (void**) &dev_b, size);
    cudaMalloc( (void**) &dev_c, size);

    cudaMemcpy(dev_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, size, cudaMemcpyHostToDevice);

    vecAdd<<<1,N>>>(dev_a, dev_b, dev_c);
    cudaMemcpy(c, dev_c, size, cudaMemcpyDeviceToHost);

    cudaFree(dev_a);
    cudaFree(dev_b);
    cudaFree(dev_c);

    printf("c -> "); print_array(c);

    exit (0);
}

__global__ void vecAdd(int* a, int* b, int* c){
    int i = threadIdx.x;
    c[i] = a[i] + b[i];
}

void print_array(int* a){
    for(int i = 0; i < N; i++){
        printf("%d ", a[i]);
    }
    printf("\n");
}
