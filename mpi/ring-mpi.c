// mpicc ring-mpi.c && mpirun --machinefile maquinas.txt -np 8 a.out

#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){
    int rank, size, tag = 0;
    int value = 0;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0){
        value = rank;
        printf("(#%3d) Começando, enviando %d para (#%d)!\n\n", rank, value, rank+1);
        MPI_Send(&value, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD);
        MPI_Recv(&value, 1, MPI_INT, size-1, tag, MPI_COMM_WORLD, &status);
        printf("(#%3d) Recebi: %d!\n\n", rank, value);
    }
    else{
        MPI_Recv(&value, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        printf("(#%3d) Recebi: %d!\n", rank, value);
        value += rank;
        printf("(#%3d) Enviando %d para (#%d)!\n\n", rank, value, rank+1);

        MPI_Send(&value, 1, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
