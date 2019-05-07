// mpic++ mandelbrot_mpi.cpp -o mandelbrot_mpi.out && mpirun -mca btl_tcp_if_include enp2s0 --machinefile ips.txt -np 8 mandelbrot_mpi.out

#include <mpi.h>
#include <complex>
#include <iostream>

using namespace std;

int main(int argc, char** argv){
	int r, c;
	int rank, size, tag = 0;
	int max_row, max_column, max_n;
    MPI_Status status;

	max_row = 23;
    max_column = 29;
	max_n = 240;

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
			MPI_Send(mat[i], (23 * 29) / size, MPI_CHAR, i, tag, MPI_COMM_WORLD);
		}
		// MPI_Scatter(mat[0], (23 * 29) / size, MPI_CHAR, 
		// 			&receive,  (23 * 29) / size, MPI_CHAR,
		// 			0, MPI_COMM_WORLD);
	}
	else{
        MPI_Recv(&receive, (23 * 29) / size, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
	}

	// for(c = 0; c < max_column; c++){
	// 	complex<float> z;
	// 	int n = 0;
	// 	while(abs(z) < 2 && ++n < max_n)
	// 		z = pow(z, 2) + decltype(z)(
	// 			(float) c * 2 / max_column - 1.5,
	// 			(float) rank * 2 / max_row - 1
	// 		);
	// 	receive[c]=(n == max_n ? '#' : '.');
	// }

	// printf("(%d) -> %c\n", rank, receive[0]);

	MPI_Finalize();

	// for(int r = 0; r < max_row; ++r){
	// 	for(int c = 0; c < max_column; ++c)
	// 		std::cout << mat[r][c];
	// 	cout << '\n';
	// }

	return 0;
}
