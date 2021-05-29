#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//include header file 
#include "mpi.h"
#include <unistd.h>
#define MASTER 0
#define REPETITIONS 10

double Function(double x);

int main(argc,argv)
int argc;
char *argv[];{
	int calc_rank, no_of_procesors,a,b,i,r,n,intervals;
	long j;
	double times[REPETITIONS];
	double calc_time, slowest, strt_pt, end_pt,result,h,x,final_result;
	//MPI initialisation
	MPI_Request request;
	int flag;
	MPI_Status status;
	
	MPI_Init(&argc,&argv);
  	MPI_Comm_rank(MPI_COMM_WORLD, &calc_rank);  
  	MPI_Comm_size(MPI_COMM_WORLD, &no_of_procesors);
	//Processor 0 sends int n to all other processors
	if (calc_rank == 0)
		{
			printf("Enter the number of intervals: \n");
			scanf("%d",&intervals);n=intervals*10;
			
			for (r=1;r<no_of_procesors;r++){
				printf("Sending n to proc %d\n",r);
				sleep(2);
				MPI_Isend(&n, 1, MPI_INT, r, 0, MPI_COMM_WORLD, &request);
			}			
		}
	//Rest of the processors receive the value of int n from the 0th processor
	else {
		MPI_Irecv(&n, 1, MPI_INT, 0, calc_rank, MPI_COMM_WORLD, &request);
		
	}MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	strt_pt = (double) 1.0 / no_of_procesors * calc_rank;
	end_pt = (double) 1.0 / no_of_procesors * ( calc_rank + 1.0);
	
	
	for ( i = 0 ; i < REPETITIONS ; ++i ){
		MPI_Barrier(MPI_COMM_WORLD);
		calc_time = MPI_Wtime();
		h = ( ( end_pt - strt_pt ) / n ) ; 
		x = strt_pt;
		result = Function(x); 
		x = x + h;
		for ( j=1l ; x < end_pt ; ++j ){
			result = ( j % 2 == 0 ) ? result + 2 * Function(x) : result + 4 * Function(x);
			x = x + h;
		}
		result += Function(x);
		result = result / ( 3.0 ) * h;
		
		//Adds all the values in parallel
		MPI_Reduce(&result,&final_result,1,MPI_DOUBLE,MPI_SUM,MASTER,MPI_COMM_WORLD);

		calc_time = MPI_Wtime() - calc_time;
		MPI_Barrier(MPI_COMM_WORLD); 
		MPI_Reduce(&calc_time, &slowest, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
		
		if (calc_rank == 0) {
	    	times[i]=slowest;
	   	}
	}
	
	if ( calc_rank == 0 ){
		double min = times[0];
	   	for (int i = 1; i < REPETITIONS ; i++) {
			min = ( times[i] < min ) ? times[i] : min;
	   	}
		printf ("The value of pi with %d intervals is %.16f\n",n/10,final_result);
	}

  	MPI_Finalize();		
}
double Function(double x){
	double result = 4.0 / (1.0 + x*x);
	return result;
}