// mpicc scatter_mpi.c -o scatter_arthur.out && mpirun --machinefile ips.txt -np 8 scatter_arthur.out
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    int *send_buffer, *receive_buffer;
    int rank, size, i;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    send_buffer = (int*) malloc(size * sizeof(int));
    if(rank == 0){
        for(i = 0; i < size; i++){
            send_buffer[i] = i * i;
            printf("(%d) generated: %d\n", rank, send_buffer[i]);
        }
    }

    MPI_Scatter(send_buffer,     1, MPI_INT, 
                &receive_buffer, 1, MPI_INT,
                0, MPI_COMM_WORLD);
    
    if(rank != 0)
        printf("(%d) received: %d\n", rank, receive_buffer);
    
    MPI_Finalize();
    return 0;
}