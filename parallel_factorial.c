#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"


int main(int argc , char * argv[])
{
	int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for 	*/
	
    int range ;
    int rem ;
    unsigned long long int  number =1;
    unsigned long long int  subFactorial =1;
    unsigned long long int  numberFactorial =1;
    unsigned long long int i  , j;
    int pNum ;
    clock_t st1, st2 ,fn1,fn2;
	double time1 , time2;


	/* Start up MPI */
	MPI_Init( &argc , &argv );

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);
///// MPI_Bcast && MPI_Reduce /////////////////////////////////////////////

	if( my_rank == 0)
	{
		printf("Hello from master process.\nNumber of slave processes is %d\nPlease enter a number: ", p-1 );
       	scanf("%llu" , &number);
       	st1 = clock();
        range = number/(p-1);
        rem = number%(p-1);
     //  printf("( %llu )!  = %llu \n",number ,numberFactorial );
	}

	//MPI_Bcast(&number, 1, MPI_UNSIGNED_LONG_LONG, 0 , MPI_COMM_WORLD );
	MPI_Bcast(&range, 1, MPI_INT, 0 , MPI_COMM_WORLD );
	
	if(my_rank !=0)
	{

		int a = ((my_rank-1)*range ) +1;
		int b = a+range;
		b--;
		for( i=a ;i <=b ;i++){
			subFactorial*=i;
		}
		//printf("p%d: calculate partial factorial from %d to %d = %llu \n",my_rank,a,b ,subFactorial);
	}
	// the master will ony do work if there is some remainder 
	if (my_rank==0 && rem > 0 )
	{
		int a = number-rem +1;
		int b = number;
		for( i=a ;i <=b ;i++){
			subFactorial*=i;
		}
		printf("p%d: calculate partial factorial from %d to %d = %llu \n",my_rank,a,b ,subFactorial);

	}

	MPI_Reduce(&subFactorial, &numberFactorial, 1, MPI_UNSIGNED_LONG_LONG, MPI_PROD,0,
              MPI_COMM_WORLD);
	
	if (my_rank==0)
	{	

		fn1 = clock();
		time1 = (double)(fn1 - st1) / CLOCKS_PER_SEC;

		printf("( %llu )!  = %llu \n",number ,numberFactorial );
	}
///// MPI_SEND && MPI_Receive /////////////////////////////////////////////
	numberFactorial=1;
	subFactorial=1;
	if( my_rank == 0)
	{
		st2 = clock();
		int pNum =0 ;
		int thisSize;
        for(pNum=1  ;pNum< p ;pNum++) {
			if(pNum<=rem ) {
				thisSize = range +1 ;				
			}
			else{
				thisSize = range ;
			}

			MPI_Send(&thisSize, 1, MPI_UNSIGNED_LONG_LONG, pNum, tag, MPI_COMM_WORLD);
			
		}

		for (pNum = 1; pNum < p; pNum++) {
			
			MPI_Recv(&subFactorial, 1, MPI_UNSIGNED_LONG_LONG, pNum, tag, MPI_COMM_WORLD, &status);
			numberFactorial*=subFactorial;

		}
		fn2 = clock();
		time2 = (double)(fn2 - st2) / CLOCKS_PER_SEC;
       printf("( %llu )!  = %llu \n",number ,numberFactorial );
	}
	else{

		MPI_Recv(&range, 1, MPI_UNSIGNED_LONG_LONG, pNum, tag, MPI_COMM_WORLD, &status);
		unsigned long long int a = ((my_rank-1)*range ) +1;
		unsigned long long int b = a+range;
		b--;
		for( i=a ;i <=b ;i++){
			subFactorial*=i;
		}
		MPI_Send(&subFactorial, 1, MPI_UNSIGNED_LONG_LONG, 0, tag, MPI_COMM_WORLD);
	}
	/* shutdown MPI */

	if (my_rank==0)
	{
		printf("MPI_Bcast && MPI_Reduce time =  %lf\n",time1 );
		printf("MPI_SEND && MPI_Receive time =  %lf\n",time2 );
	}
	MPI_Finalize();
	return 0;
}
