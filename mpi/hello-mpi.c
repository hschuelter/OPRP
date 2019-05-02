// mpicc hellop-mpi.c && mpirun --machinefile maquinas.txt -np 8 a.out

#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){
    int rank, size, name_len, i, tag = 0;
    char processor_name[MPI_MAX_PROCESSOR_NAME], msg[20];
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Get_processor_name(processor_name, &name_len);


    if(rank == 0){
        printf("Hello, I am %s\n", processor_name);
        strcpy(msg, "Hello");
        for(i = 1; i < size; i++){
            MPI_Send(msg, 20, MPI_CHAR, i, tag, MPI_COMM_WORLD);
        }
    }
    else{
        printf("(%d) Estou esperando...\n", rank);
        MPI_Recv(msg, 20, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
        printf("(%d) Recebi!\n\n", rank);
    }

    MPI_Finalize();

    return 0;
}
