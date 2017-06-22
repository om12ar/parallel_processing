#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "mpi.h"

/////////////////////////////////
//	Omar Ibrahim Saber
//	20130316
//	CS4
/////////////////////////////////


int main(int argc , char * argv[])
{
	int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for 	*/
	int * arr ; 
    int Pmax ;
    int size ;
    int i  , j;
    int pNum ;
    

	/* Start up MPI */
	MPI_Init( &argc , &argv );

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if( my_rank == 0)
	{
		printf("Hello from master process.\nNumber of slave processes is %d\nPlease enter size of array: ", p-1 );
        
        scanf("%d" , &size);
        printf("Please enter array elements: \n");
        arr = malloc(sizeof(int) * size);
        for(i =0 ; i < size ; i++){
            scanf("%d" , &arr[i]);
        }

        int subArraySize = size/(p-1) ; 
        int remElements = size % (p-1) ; 
        int thisSize ;
        i=0; 
        for(pNum=1  ;pNum< p ;pNum++) {
			if(pNum<=remElements ) {
				thisSize = subArraySize +1 ;				
			}
			else{
				thisSize = subArraySize ;
			}

			MPI_Send(&thisSize, 1, MPI_INT, pNum, tag, MPI_COMM_WORLD);
			int *subArr = malloc(sizeof(int) * thisSize);
			for ( j = 0; i < size && j< thisSize; j++, i++){
				
				subArr[j] = arr[i];			
			}
			

			MPI_Send(subArr, thisSize, MPI_INT, pNum, tag, MPI_COMM_WORLD);		
			
		}
		int Max = arr[0];
		int maxIdx=0; 
		for (pNum = 1; pNum < p; pNum++) {
			//printf("%d\n",pNum );
			int subMax ;
			int subMaxIdx ;
			MPI_Recv(&subMax, 1, MPI_INT, pNum, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&subMaxIdx, 1, MPI_INT, pNum, tag, MPI_COMM_WORLD, &status);
			if (subMax > Max){
				Max = subMax;
				maxIdx = subMaxIdx + ((pNum-1)*subArraySize) ;
				if(remElements!=0){
					if(remElements<pNum){
						maxIdx+=remElements;
					}else{
						maxIdx += pNum-1;
					}
				}
			}

		}
 		
		printf("Master process announce the final max which is %d and its index is %d.\n",Max , maxIdx);
	}else
	{

		int subArraySize;
		MPI_Recv(&subArraySize, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		int * subArr = malloc(sizeof(int) * subArraySize);
		MPI_Recv(subArr, subArraySize, MPI_INT, 0, tag, MPI_COMM_WORLD,
				&status);		
        int subMax = INT_MIN ;
        int subMaxIdx = 0 ;
		for (i = 0; i < subArraySize; i++) {
			if (subArr[i] > subMax){
				subMax = subArr[i];
				subMaxIdx = i ;
			}
				
		}
		printf("Hello from slave#%d Max number in my partition is %d and index is %d.\n",
		 my_rank , subMax , subMaxIdx );
		fflush(stdout);
		MPI_Send(&subMax, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
		MPI_Send(&subMaxIdx, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	
	}


	/* shutdown MPI */
	MPI_Finalize();
	return 0;
}
