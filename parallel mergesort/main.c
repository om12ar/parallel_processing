#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

    void mergeSortCall(int *a, int left, int right)
    {
        int *tmp = malloc ((right - left)* sizeof (int));
        mergeSort(a, tmp, left,  right - 1);
    }
    void mergeSort(int *array, int  *temp, int left, int right)
    {   
        if( left < right )
        {
            //printf("left %d right %d\n", left , right);
            int middle = (left + right) / 2;
            mergeSort(array, temp, left, middle);
            mergeSort(array, temp, middle + 1, right);
            merge(array, temp, left, middle + 1, right);
        }
    }

    void merge(int *array, int *temp, int left, int right, int rightEdge )
    {
        
        int leftEdge = right - 1;
        int k = left;
        int num = rightEdge - left + 1;

        while(left <= leftEdge && right <= rightEdge)
            if(array[left] <= array[right]){
                
                temp[k] = array[left];
                k++;
                left++;
            }
            else{
                
                temp[k] = array[right];
                k++;
                right++;
            }
        
        while(left <= leftEdge){   
            temp[k] = array[left];
            k++;
            left++;

        }
        
        while(right <= rightEdge){  
            temp[k] = array[right];
            k++;
            right++;
        }
       
        int i;
        
        for(i = 0; i < num; i++, rightEdge--){
            array[rightEdge] = temp[rightEdge];
        }
    }


int main(int argc , char * argv[])
{
    int rank , p , subSize , rem;
    MPI_Status st;
    MPI_Init( &argc , &argv );
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    int i , size , j , it; 
    int *array , *temp;
    int start , end;
// Handle Input 
    if (rank == 0)
    {
        printf("Welcome to parallel merge sort\n");
        printf("Enter size of the array\n");
        scanf("%d" , &size);
        printf("Enter array elements\n");
        array = malloc(size * sizeof(int));
        for (i = 0 ; i < size ;i++)
            scanf("%d" , &array[i]);

        subSize = size / p ; 
        rem = size % p;
    }

    MPI_Bcast(&subSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int *subArray ;
    subArray = malloc(subSize * sizeof(int));

    MPI_Scatter(array, subSize, MPI_INT, subArray, subSize, MPI_INT, 0, MPI_COMM_WORLD);
    
    start =  0 ;
    end = subSize ;

    mergeSortCall(subArray , start ,end);

    MPI_Gather(subArray, subSize, MPI_INT, array , subSize, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0) {

        //Sort the remainder elements 
        mergeSortCall(array , size - rem, size - 1);
       
        // final merge call to merge the outputs of all processes 
        temp = malloc(size * sizeof(int));
        for(i = 1; i < p ; i++) {
          merge(array , temp , 0 , i * subSize , ((i + 1) * subSize) - 1);
        }
        // merge the remainder 
        merge(array , temp , 0 , size - rem, size - 1);


        printf("This is the sorted array: ");
        for (i = 0 ; i < size ; i++)
            printf("%d ", array[i]);
            
            
    }
    MPI_Finalize();
}
