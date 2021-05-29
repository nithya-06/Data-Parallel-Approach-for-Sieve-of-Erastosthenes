#include <stdio.h>
#include "mpi.h"
#include<stdlib.h>
//declare the variable and functions
int val_size = 0;
int val = 0;
//separate routine for SHIFT
static void Shiftarray(int org_arr[], int res_arr[], int shift_fac, int shift_arr[]);
static void Right_shift(int org_arr[], int res_arr[], int shift_fac, int shift_arr[]);
static void Left_shift(int org_arr[], int res_arr[], int shift_fac, int shift_arr[]);
//define all the functions 
void shiftArray(int org_arr[], int res_arr[], int shift_fac, int shift_arr[])
{
    int temp_arr[val];

    if (shift_fac >= 0)
    {
        Right_shift(org_arr, res_arr, shift_fac, shift_arr);
    }

    if (shift_fac < 0)
    {
        Left_shift(org_arr, res_arr, shift_fac, shift_arr);
    }
}
void Right_shift(int org_arr[], int res_arr[], int shift_fac, int shift_arr[])
{
    int i = 0, j = 0;
    for (j = 0; j <= shift_fac; j++)
    {
        res_arr[j] = org_arr[i];
    }
    for (i = 1; i < (val - shift_fac); i++, j++)
    {
        res_arr[j] = org_arr[i];
    }

    for (i = 1; i <= shift_fac; i++)
    {
        shift_arr[i - 1] = org_arr[val - shift_fac + i - 1];
    }
}
void Left_shift(int org_arr[], int res_arr[], int shift_fac, int shift_arr[])
{
    int i = 0, j = 0;
    i = -1;
    for (j = 0; j <= (val + shift_fac); j++)
    {
        res_arr[j] = org_arr[(shift_fac * i) + j];
    }
    for (j = 0; j <= (shift_fac * i); j++)
    {
        shift_arr[j] = org_arr[j];
    }
}
//Main program
int main(int cvalue, char *valuev[])
{
    int shift_fac;
    int new_pos;
    int elem;
    int i = 0;
    int processor_rnk;
    int val_size;
    //MPI INITIALISATION
    MPI_Status status;
    MPI_Init(&cvalue, &valuev);
    MPI_Comm_rank(MPI_COMM_WORLD, &processor_rnk);
    MPI_Comm_size(MPI_COMM_WORLD, &val_size);

    if ((val_size == 2) || (val_size == 4) || (val_size == 8) || (val_size == 16))
    {
    }
    else
    {
        MPI_Abort(MPI_COMM_WORLD, i);
        exit(0);
    }
    if (processor_rnk == 0)
    {
        printf("Enter input Array Size:\n");
        scanf("%d", &val);
        printf("\nEnter the Shift factor ");
        scanf("%d", &shift_fac);
        int input_array[val];
        int res_arr[val];
        int shift_arr[val];
        printf("Enter the rank of processor : %d\n", processor_rnk);
        for (i = 0; i < val; i++)
        {
            printf("\nArr[%d]:", i);
            scanf("%d", &input_array[i]);
        }
        if (shift_fac < 0)
        {
            while (shift_fac < (-val))
            {
                shift_fac = shift_fac + val;
            }
        }
        else if (shift_fac > 0)
        {
            while (shift_fac > val)
            {
                shift_fac = shift_fac - val;
            }
        }

        if (val_size > 1)
        {
            for (elem = 1; elem < val_size; elem++)
            {
                MPI_Send(&shift_fac, 1, MPI_INT, elem, elem, MPI_COMM_WORLD);
                MPI_Send(&val, 1, MPI_INT, elem, elem, MPI_COMM_WORLD);
                printf("Enter the rank of processor : %d\n", elem);
                for (i = 0; i < val; i++)
                {
                    printf("\nArr[%d]:", i);
                    scanf("%d", &res_arr[i]);
                }
                MPI_Send(&res_arr, val, MPI_INT, elem, elem, MPI_COMM_WORLD);
            }
        }

        shiftArray(input_array, res_arr, shift_fac, shift_arr);

        if (shift_fac < 0)
        {
            new_pos = (-shift_fac);
        }
        else
        {
            new_pos = shift_fac;
        }

        if (val_size > 1)
        {
            if (shift_fac >= 0)
            {
                MPI_Send(&shift_arr, new_pos, MPI_INT, 1, 1, MPI_COMM_WORLD);
                MPI_Recv(&shift_arr, new_pos, MPI_INT, (val_size - 1), (val_size - 1), MPI_COMM_WORLD, &status);
            }
            else
            {
                MPI_Send(&shift_arr, new_pos, MPI_INT, (val_size - 1), (val_size - 1), MPI_COMM_WORLD);
                MPI_Recv(&shift_arr, new_pos, MPI_INT, 1, processor_rnk, MPI_COMM_WORLD, &status);
            }
        }
        if (shift_fac >= 0)
        {
            for (i = 0; i < shift_fac; i++)
            {
                res_arr[i] = shift_arr[i];
            }
        }
        if (shift_fac < 0)
        {
            for (i = 0, elem = (val + shift_fac); elem < val; i++, elem++)
            {
                res_arr[elem] = shift_arr[i];
            }
        }
        printf("\nAfter shift,the output is :::");
        printf("\n");
        //PRINT the output
        for (i = 0; i < val; i++)
        {
            printf("\nOriginal Array : %d, Result Array  is : %d\n", input_array[i], res_arr[i]);
        }
        if (val_size > 1)
        {
            for (elem = 1; elem < val_size; elem++)
            {
                MPI_Send(&shift_fac, 1, MPI_INT, elem, elem, MPI_COMM_WORLD);
                MPI_Recv(&shift_fac, 1, MPI_INT, elem, processor_rnk, MPI_COMM_WORLD, &status);
            }
        }
    }
    else if (processor_rnk != 0)
    {
        MPI_Recv(&shift_fac, 1, MPI_INT, 0, processor_rnk, MPI_COMM_WORLD, &status);
        MPI_Recv(&val, 1, MPI_INT, 0, processor_rnk, MPI_COMM_WORLD, &status);
        int input_array[val];
        int res_arr[val];
        int shift_arr[val];
        int temp_arr[val];

        if (shift_fac < 0)
        {
            new_pos = shift_fac * -1;
        }
        else
        {
            new_pos = shift_fac;
        }

        MPI_Recv(&input_array, val, MPI_INT, 0, processor_rnk, MPI_COMM_WORLD, &status);
        shiftArray(input_array, res_arr, shift_fac, shift_arr);
        if (shift_fac >= 0)
        {
            MPI_Recv(&temp_arr, new_pos, MPI_INT, (processor_rnk - 1), processor_rnk, MPI_COMM_WORLD, &status);
            for (i = 0; i < shift_fac; i++)
            {
                res_arr[i] = temp_arr[i];
            }
        }
        if (shift_fac < 0)
        {
            if (processor_rnk == (val_size - 1))
            {
                MPI_Recv(&temp_arr, new_pos, MPI_INT, 0, processor_rnk, MPI_COMM_WORLD, &status);
            }
            else
            {
                MPI_Recv(&temp_arr, new_pos, MPI_INT, (processor_rnk + 1), processor_rnk, MPI_COMM_WORLD, &status);
            }
            for (i = 0, elem = (val + shift_fac); elem < val; i++, elem++)
            {
                res_arr[elem] = temp_arr[i];
            }
        }

        if (processor_rnk == (val_size - 1))
        {
            if (shift_fac >= 0)
            {
                MPI_Send(&shift_arr, new_pos, MPI_INT, 0, processor_rnk, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Send(&shift_arr, new_pos, MPI_INT, (processor_rnk - 1), (processor_rnk - 1), MPI_COMM_WORLD);
            }
        }
        else
        {
            if (shift_fac >= 0)
            {
                MPI_Send(&shift_arr, new_pos, MPI_INT, (processor_rnk + 1), (processor_rnk + 1), MPI_COMM_WORLD);
            }
            else
            {
                MPI_Send(&shift_arr, new_pos, MPI_INT, (processor_rnk - 1), (processor_rnk - 1), MPI_COMM_WORLD);
            }
        }
        MPI_Recv(&shift_fac, 1, MPI_INT, 0, processor_rnk, MPI_COMM_WORLD, &status);
        for (i = 0; i < val; i++)
        {
            printf("Original Array: %d, Result Array  : %d \n", input_array[i], res_arr[i]);
        }
        MPI_Send(&shift_fac, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}