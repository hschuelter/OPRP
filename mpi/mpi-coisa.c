#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv){
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Hello, I'm %d of %d\n", rank, size);

    if(rank == 0){
        printf("(%d) -> primeiro a escrever\n", rank);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    else{
        MPI_Barrier(MPI_COMM_WORLD);
        printf("(%d) -> agora posso escrever\n", rank);
    }

    MPI_Finalize();

    return 0;
}
