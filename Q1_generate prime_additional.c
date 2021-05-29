#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
/*MAIN FUNCTION*/
int main(int argc, char** argv)
{
  int c_rank;
  int no_of_proc;

  //MPI INITIALIZATION
  MPI_Init(&argc, &argv);
//c_rank is id of proc 
  MPI_Comm_rank(MPI_COMM_WORLD, &c_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &no_of_proc);
  if (argc < 3)
  {
    if (c_rank == 0)
    {
      printf("Provide enough arguements.\nExiting.\n");
    }
    exit(1);
  }
  if (c_rank == 0)
  {
    printf("Finding all primes in the range from 0 to %s with %s processors.\n", argv[2], argv[1]);
  }
  int* new_list;
  int* splitted_list;
  int split_ratio = 0;
  int remainder = 0;
  int low   = 0;
  int high  = 0;
  int n = atoi(argv[2]);
  split_ratio = (n - (sqrt(n) + 1)) / no_of_proc;
  remainder = ((int) (n - (sqrt(n) + 1))) % no_of_proc;
  low = sqrt(n) + c_rank * split_ratio + 1;
  high = low + split_ratio - 1;
  if (c_rank == no_of_proc - 1)
  {
    high += remainder;
  }

  new_list = (int*) calloc(1, sqrt(n + 1) * sizeof(int));
  splitted_list = (int*) calloc(1, (high - low + 1) * sizeof(int));

  int ind_1;
  for (ind_1 = 2; ind_1 <= sqrt(n); ind_1++)
  {
    if (new_list[ind_1] == 0)
    {
      int ind_2;
      for (ind_2 = ind_1 + 1; ind_2 <= sqrt(n); ind_2++)
      {
        if (ind_2 % ind_1 == 0)
        {
          new_list[ind_2] = 1;
        }
      }
      for (ind_2 = low; ind_2 <= high; ind_2++)
      {
        if (ind_2 % ind_1 == 0)
        {
          splitted_list[ind_2 - low] = 1;
        }
      }
    }
  }
  if (c_rank == 0)
  {
    printf("\nThe primes numbers are: ");
    for (ind_1 = 2; ind_1 <= sqrt(n); ind_1++)
    {
      if (new_list[ind_1] == 0)
      {
        printf("%d ", ind_1);
      }
    }
    for (ind_1 = low; ind_1 <= high; ind_1++)
    {
      if (splitted_list[ind_1 - low] == 0)
      {
        printf("%d ", ind_1);
      }
    }
    int* buffer = (int*) calloc(1, n * sizeof(int));
    // Go through the other processes and calculate low,high
    for (c_rank = 1; c_rank <= no_of_proc - 1; c_rank++)
    {
      low = sqrt(n) + c_rank * split_ratio + 1;
      high = low + split_ratio - 1;

      if (c_rank == no_of_proc - 1)
      {
        high += remainder;
      }
      //get multiples 
      MPI_Recv(buffer, high - low + 1, MPI_INT, c_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for (ind_1 = low; ind_1 <= high; ind_1++)
      {
        
        if (buffer[ind_1 - low] == 0)
        {
          printf("%d ", ind_1);
        }
      }
    }
    printf("\n\n\n\n");
  }
  else
  {
    
    MPI_Send(splitted_list, high - low + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

 
  MPI_Finalize();

  return 0;
}
