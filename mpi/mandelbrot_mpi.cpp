// mpic++ mandelbrot_mpi.cpp -o mandelbrot_mpi.out && mpirun -mca btl_tcp_if_include enp2s0 --machinefile m.txt -np 8 mandelbrot_mpi.out

#include <mpi.h>
#include <complex>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv){
	int r, c;
	int rank, size, tag = 0;
	int max_row, max_column, max_n;
	int begin, end;
	ofstream file;

	file.open("mandel.txt");


    MPI_Status status;

	max_row = 20;
    max_column = 40;
	max_n = 150;

	char** mat = (char**) malloc( sizeof(char*) * max_row);
	char* aux  = (char*)  malloc( sizeof(char)  * max_row * max_column);
	char* receive;

	for (int i=0; i < max_row; i++)
		mat[i] = &aux[i * max_column];


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if(rank == 0){
		for(int i =1; i < size; i++){
			begin = (int)   i * (1.0 * max_row / size);
			end  = 	(int) (i+1) * (1.0 * max_row / size);
			// printf("Enviei b: %d e end: %d para o rank %d\n", begin, end, i);
			MPI_Send(&begin, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			MPI_Send(&end, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
		}
		begin = 0;
		end  = 	(int) (1.0 * max_row / size);

		printf("(%d) Eu tenho b: %d e end: %d\n", rank, begin, end);


		for(int r = begin; r < end; r++){
			for(int c = 0; c < max_column; c++){
				complex<float> z;
				int n = 0;
				while(abs(z) < 2 && ++n < max_n)
				z = pow(z, 2) + decltype(z)(
					(float) c * 2 / max_column - 1.5,
					(float) r * 2 / max_row - 1
				);
				mat[r][c] = (n == max_n ? '#' : '.');
			}
		}

		for(int i =1; i < size; i++){
			MPI_Recv(&begin, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
	        MPI_Recv(&end, 	1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);

			for(int j=begin; j < end; j++){
				MPI_Recv(mat[j], max_column, MPI_CHAR, i, tag, MPI_COMM_WORLD, &status);
				printf("RECEIVING %d\n", j);
			}
		}
	}
	else{
		MPI_Recv(&begin, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&end, 	1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

		printf("(%d) tenho b: %d e end: %d\n", rank, begin, end);

		receive = (char*) malloc(sizeof(char) * (end  - begin));
		int pos = 0;
		for(int r = begin; r < end; r++){
			for(int c = 0; c < max_column; c++){
				complex<float> z;
				int n = 0;
				while(abs(z) < 2 && ++n < max_n)
				z = pow(z, 2) + decltype(z)(
					(float) c * 2 / max_column - 1.5,
					(float) r * 2 / max_row - 1
				);
				receive[pos] = (n == max_n ? '#' : '.');
				pos++;
			}
		}
		MPI_Send(&begin, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		MPI_Send(&end, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		printf("(%d) Enviei b: %d e end: %d para o rank %d\n", rank, begin, end, 0);

		for(int i = 0; i < (end-begin); i++){
			MPI_Send(&receive[i * (max_column)], max_column, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
			printf("SENDING %d\n", i + begin);
		}
	}


	// printf("(%d) -> %c\n", rank, receive[0]);

	MPI_Finalize();

	for(int r = 0; r < max_row; ++r){
		for(int c = 0; c < max_column; ++c)
			file << mat[r][c];
		file << '\n';
	}

	return 0;
}
