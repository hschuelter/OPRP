// nvcc sum_matrix.cu -o sum_matrix.out
#include <stdio.h>
#define N 10

void print_matrix(int** mat);

int main(void){
    int **a, **b, **c;
    int **d_a, **d_b, **d_c;

    a = (int**) malloc(sizeof(int*) * N);
    b = (int**) malloc(sizeof(int*) * N);
    c = (int**) malloc(sizeof(int*) * N);
    for(int i = 0; i < N; i++){
        a[i] = (int*) malloc(sizeof(int) * N);
        b[i] = (int*) malloc(sizeof(int) * N);
        c[i] = (int*) malloc(sizeof(int) * N);
    }

    printf("a: \n"); print_matrix(a);
    printf("b: \n"); print_matrix(b);
    printf("c: \n"); print_matrix(c);

    exit(0);
}

void print_matrix(int** mat){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
}

// matrix_t *matrix_sum_serial(matrix_t *A, matrix_t *B){
//     //Não podemos somar
//     if( (A->rows != B->rows) && (A->cols != B->cols) ){
//         printf("Impossível somar\n");
//         return NULL;
//     }
//
//     int i,j;
//     for(i = 0; i < A->rows; i++){
//         for(j = 0; j < A->cols; j++){
//             A->data[i][j] += B->data[i][j];
//         }
//     }
//
//     return A;
// }
