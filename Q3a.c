#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
/*main function*/
int main(int argc, char *argv[])
{
	//MPI INITIALIZATION
	MPI_Init(&argc, &argv);
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int c_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &c_rank);
	MPI_Status stat;
	if(size != 2){
		if(c_rank == 0){
			printf("Program needs only 2 MPI c_rank.you are using %d ranks!.Exit\n", size);
		}
		MPI_Finalize();
		exit(0);
	}
	
/*VARYING THE DATA SIZE between 0 bytes to 512 bytes, with increments of 32 bytes*/
	for(int i=0; i<=15; i++){
		long int N = 1 + i;
		// MEMORY ALLOCATION FOR ARRAY A
		double *A = (double*)malloc(N*sizeof(double));
		for(int i=0; i<N; i++){
			A[i] = 0.0;
		}
		int t1 = 10;
		int t2 = 20;
		//loop count 
		int c = 50;
		for(int i=1; i<=5; i++){
			if(c_rank == 0){
				MPI_Send(A, N, MPI_DOUBLE, 1, t1, MPI_COMM_WORLD);
				MPI_Recv(A, N, MPI_DOUBLE, 1, t2, MPI_COMM_WORLD, &stat);
			}
			else if(c_rank == 1){
				MPI_Recv(A, N, MPI_DOUBLE, 0, t1, MPI_COMM_WORLD, &stat);
				MPI_Send(A, N, MPI_DOUBLE, 0, t2, MPI_COMM_WORLD);
			}
		}
		//Communication Time  for ping-pong for c iterations of data transfer size 8*N bytes
		double strt_time, end_time, elapsed_time,sum_time,avg_time,latency;
		strt_time = MPI_Wtime();
	
		for(int i=1; i<=c; i++){
			if(c_rank == 0){
				MPI_Send(A, N, MPI_DOUBLE, 1, t1, MPI_COMM_WORLD);
				MPI_Recv(A, N, MPI_DOUBLE, 1, t2, MPI_COMM_WORLD, &stat);
			}
			else if(c_rank == 1){
				MPI_Recv(A, N, MPI_DOUBLE, 0, t1, MPI_COMM_WORLD, &stat);
				MPI_Send(A, N, MPI_DOUBLE, 0, t2, MPI_COMM_WORLD);
			}
		}
		end_time = MPI_Wtime();
		elapsed_time = end_time - strt_time;
		sum_time += elapsed_time;
		avg_time = (sum_time*1000000)/i;
		latency = avg_time/2;
		long int data_size = 32*N;
		long int Byt_gB = 1 << 30;
		double no_gB = (double)data_size / (double)Byt_gB;
		double comm_time = elapsed_time / (1.0*(double)c);
		if(c_rank == 0) printf("%10li\t%15.9f\t%15.9f\n", data_size, comm_time,latency);

		free(A);
	}
	printf("\n");
	/*VARYING THE DATA SIZE between 1kbytes to 128kbytes, with increments of 1k*/
	for(int i=0; i<=127; i++){

		long int N = 1 + i;
	
   	 	// Allocate memory for A on CPU
		double *A = (double*)malloc(N*sizeof(double));

		// Initialize all elements of A to 0.0
		for(int i=0; i<N; i++){
			A[i] = 0.0;
		}
	
		int t1 = 10;
		int t2 = 20;
		int c = 50;
		for(int i=1; i<=5; i++){
			if(c_rank == 0){
				MPI_Send(A, N, MPI_DOUBLE, 1, t1, MPI_COMM_WORLD);
				MPI_Recv(A, N, MPI_DOUBLE, 1, t2, MPI_COMM_WORLD, &stat);
			}
			else if(c_rank == 1){
				MPI_Recv(A, N, MPI_DOUBLE, 0, t1, MPI_COMM_WORLD, &stat);
				MPI_Send(A, N, MPI_DOUBLE, 0, t2, MPI_COMM_WORLD);
			}
		}

		// Time ping-pong for c iterations of data transfer size 1000*N bytes
		double strt_time, end_time, elapsed_time,sum_time,avg_time,latency;
		strt_time = MPI_Wtime();
	
		for(int i=1; i<=c; i++){
			if(c_rank == 0){
				MPI_Send(A, N, MPI_DOUBLE, 1, t1, MPI_COMM_WORLD);
				MPI_Recv(A, N, MPI_DOUBLE, 1, t2, MPI_COMM_WORLD, &stat);
			}
			else if(c_rank == 1){
				MPI_Recv(A, N, MPI_DOUBLE, 0, t1, MPI_COMM_WORLD, &stat);
				MPI_Send(A, N, MPI_DOUBLE, 0, t2, MPI_COMM_WORLD);
			}
		}
		//calculate the latency and communication time for ping pong algorithm
		end_time = MPI_Wtime();
		elapsed_time = end_time - strt_time;
		sum_time += elapsed_time;
		avg_time = (sum_time*1000000)/i;
		latency = avg_time/2;
		//1000k increments
		long int data_size = 1000*N;
		long int Byt_gB = 1 << 10;
		double no_gB = (double)data_size / (double)Byt_gB;
		double comm_time = elapsed_time / (1.0*(double)c);

		if(c_rank == 0) printf("%10li\t%15.9f\t%15.9f\n", data_size, comm_time,latency);

		free(A);
	}

	MPI_Finalize();

	return 0;
}

