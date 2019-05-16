#include<stdio.h>

__device__ const char *STR = "Hello World!\n";
const char STR_LENGTH = 12;

__global__ void hello(){
    printf("%c\n", STR[threadIdx.x % STR_LENGTH]);
}

int main(void){
    hello<<<1, STR_LENGTH>>>();
    cudaDeviceSynchronize();


    return 0;
}
