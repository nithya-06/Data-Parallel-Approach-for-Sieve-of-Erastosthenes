#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//include header file
#include "mpi.h"
#define MASTER 0
#define REPETITIONS 10
double Function(double x);
int main(argc,argv)
int argc;
char *argv[];{
	int calc_rank, no_of_processors,a,b,i,r,n,intervals;
	long j;
	double times[REPETITIONS];
	double c_time, c_slowest_tme, strt_pt, end_pt,result,h,x,final_res;
	/*MPI INITIALISATION*/
	MPI_Init(&argc,&argv);
  	MPI_Comm_rank(MPI_COMM_WORLD, &calc_rank);  
  	MPI_Comm_size(MPI_COMM_WORLD, &no_of_processors);
	//Processor 0 sends int n to all other processors
	if (calc_rank == 0)
		{
			printf("Enter the number of intervals: \n");
			scanf("%d",&n);
			for (r=1;r<no_of_processors;r++){
				printf("Sending n to proc %d\n",r);
				MPI_Send(&n, 1, MPI_INT, r, r, MPI_COMM_WORLD);
			}			
		}
		//Rest of the processors receive the value of int n from the 0th processor
	else {
		MPI_Recv(&n, 1, MPI_INT, 0, calc_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE) ;
	}
	strt_pt = (double) 1.0 / no_of_processors * calc_rank;
	end_pt = (double) 1.0 / no_of_processors * ( calc_rank + 1.0);
	for ( i = 0 ; i < REPETITIONS ; ++i ){
		MPI_Barrier(MPI_COMM_WORLD);
		c_time = MPI_Wtime();
		
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
		MPI_Reduce(&result,&final_res,1,MPI_DOUBLE,MPI_SUM,MASTER,MPI_COMM_WORLD);
		c_time = MPI_Wtime() - c_time;
		MPI_Barrier(MPI_COMM_WORLD); 
		MPI_Reduce(&c_time, &c_slowest_tme, 1, MPI_DOUBLE, MPI_MAX, MASTER, MPI_COMM_WORLD);
		if (calc_rank == 0) {
	    	times[i]=c_slowest_tme;
	   	}
	}
	
	if ( calc_rank == 0 ){
		double min = times[0];
	   	for (int i = 1; i < REPETITIONS ; i++) {
			min = ( times[i] < min ) ? times[i] : min;
	   	}
		printf ("The value of pi with %d intervals is %.16f\n",n,final_res);
	}

  	MPI_Finalize();		
}
double Function(double x){
	double result = 4.0 / (1.0 + x*x);
	return result;
}