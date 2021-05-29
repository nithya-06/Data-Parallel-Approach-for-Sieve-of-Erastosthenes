/*DATA PARALLEL APPROACH using DISTRIBUTED NON-SHARED MEMORY for SIEVE of Erastosthenes*/
/*MPI is used for message passing*/

/*GIVEN:
INPUTS:
1.largest number, n
2.number of processors ,p
3.number of cores 1 to 32.
OUTPUTS:
curr_prime numbers */

/*"MyMPI.h"*/
/*BLOCK_FIRST will store the first multiple and current prime number 
 BLOCK_STEP for iterating odd nos 
 BLOCK_LOW to store lowest index
 BLOCK_HIGH to store highest index
 BLOCK_SIZE identifies no of elements
 BLOCK_OWNER to identify the group.*/
#define BLOCK_STEP 2 
#define BLOCK_FIRST 3  
#define BLOCK_HIGH(p_id, p, n) \
        (BLOCK_LOW((p_id) + 1, p, n) - 1)
#define BLOCK_LOW(p_id, p, n) \
        ((p_id) * (n) / (p))
#define BLOCK_SIZE(p_id, p, n) \
	(BLOCK_LOW((p_id) + 1, p, n) - BLOCK_LOW((p_id), p, n))
#define BLOCK_OWNER(index, p, n) \
        (((p) * ((index) + 1) - 1) / (n))/*
 *  Parallel Sieve of Eratosthenes*/
//Include the mpi library
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
int main(int argc, char *argv[]) {
    int    local_count_pnos;         
    double elaps_time;   
    int    frst_multiple;         
    int    global_count_pnos;  
    int    no_loops;             
    int    p_id;           
    int    index;         
	int    h_val;    
    int    l_val;     
    char  *ptr_mark_nos;       
    int    n;            
    int    p;             
    int    proc0_size;    
    int    curr_prime;         
    int    size;          

    MPI_Init(&argc, &argv);
    MPI_Barrier(MPI_COMM_WORLD); 
    elaps_time = -MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &p_id); 
    MPI_Comm_size(MPI_COMM_WORLD, &p);	
    //Check the number of arguements
    if (argc != 2) {	
	if (!p_id) printf("Command line:  %s <m>\n", argv[0]);
	MPI_Finalize();		
	exit(1);
    }
    //convert character to integer
    n = atoi(argv[1]); 
    l_val  = 2 + BLOCK_LOW(p_id, p, n - 1);	
	h_val = 2 + BLOCK_HIGH(p_id, p, n - 1);	
    size       = BLOCK_SIZE(p_id, p, n - 1);		
    proc0_size = (n - 1) / p;
    if ((2 + proc0_size) < (int) sqrt((double) n)) {
	if (!p_id) printf("Too many processes\n");
	MPI_Finalize();
	exit(1);
    }
    //Allocate the memory for the process
    ptr_mark_nos = (char*)malloc(size * sizeof(char));;
    if (ptr_mark_nos == NULL) {
	printf("Cannot allocate enough memory\n");
	MPI_Finalize();
	exit(1);
    }
    for (no_loops = 0; no_loops < size; no_loops++) ptr_mark_nos[no_loops] = 0;
    if (!p_id) index = 0;
    curr_prime = 2;
    do {
	if (curr_prime * curr_prime > l_val)
	    frst_multiple = curr_prime * curr_prime - l_val;
	else {
	    if (!(l_val % curr_prime)) frst_multiple = 0;
	    else frst_multiple = curr_prime - (l_val % curr_prime);
	}
	for (no_loops = frst_multiple; no_loops < size; no_loops += curr_prime) ptr_mark_nos[no_loops] = 1;
	if (!p_id) {
	    while (ptr_mark_nos[++index]);
	    curr_prime = index + 2;
	}
	MPI_Bcast(&curr_prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } while (curr_prime * curr_prime <= n);
    local_count_pnos = 0;
    for (no_loops = 0; no_loops < size; no_loops++)
	if (!ptr_mark_nos[no_loops]) local_count_pnos++;
    MPI_Reduce(&local_count_pnos, &global_count_pnos, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    //Stop the timer 
    elaps_time += MPI_Wtime();
    //print the number of primes and the time 
    if (!p_id) {
	printf("\nThere are %d prime numbers less than or equal to  %d\n", global_count_pnos,n);
	printf("\nTotal elapsed time: %10.6f\n", elaps_time);
    }
    MPI_Finalize();
    return 0;
}