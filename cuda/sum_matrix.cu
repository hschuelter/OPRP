// nvcc sum_matrix.cu -o sum_matrix.out
#include <stdio.h>
#define N 10

void print_matrix(int** mat);
int **make_matrix(int size);
void matrix_sum_serial(int **A, int **B, int **C);
__global__ void matrix_sum_cuda(int** a, int** b, int** c);

int main(void){
    int **a, **b, **c;
    int **d_a, **d_b, **d_c;

    a = make_matrix(N);
    b = make_matrix(N);
    c = make_matrix(N);

    
    cudaMalloc( (void**) &d_a, N);
    cudaMalloc( (void**) &d_b, N);
    cudaMalloc( (void**) &d_c, N);
    
    cudaMemcpy(d_a, a, N * N, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, N * N, cudaMemcpyHostToDevice);
    cudaMemcpy(d_c, c, N * N, cudaMemcpyHostToDevice);
    
    
    // matrix_sum_serial(a, b, c);
    matrix_sum_cuda<<<N,N>>>(d_a, d_b, d_c);
    cudaMemcpy(c, d_c, N * N, cudaMemcpyDeviceToHost);


    printf("a: \n"); print_matrix(a);
    printf("b: \n"); print_matrix(b);
    printf("c: \n"); print_matrix(c);

    exit(0);
}

int **make_matrix(int size){
    int** mat = (int**) malloc(sizeof(int*) * size);
    mat[0] = (int*) malloc(sizeof(int) * size * size);
    for(int i = 0; i < size; i++){
        mat[i] = &mat[0][size * i];
        for(int j = 0; j < size; j++){
            mat[i][j] = rand() % size;
        }
    }

    return mat;
}

void print_matrix(int** mat){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

void matrix_sum_serial(int **A, int **B, int **C){
    int i,j;
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            C[i][j] = A[i][j] + B[i][j];
        }
    }
}

__global__ void matrix_sum_cuda(int** a, int** b, int** c){
    int j = blockDim.x * blockIdx.x + threadIdx.x;
    int i = blockDim.y * blockIdx.y + threadIdx.y;

    c[i][j] = a[i][j] + b[i][j];
}

