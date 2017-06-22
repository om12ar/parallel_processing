#include <stdio.h>
#include <time.h>
#include <string.h>
#include "mpi.h"
#define _CRT_SECURE_NO_WARNINGS
/*
Omar Ibrahim Saber 
20130316    
CS4

mat1 is scattered by rows , all processes gets 1 or more rows
if p > r1 : the extra processes gets terminated 
if there is remainder rows each process received an extra row until the remainder is finished 

mat2 is Broadcasted to all processes

in the pdf "Input matrix can be taken from file OR console." so i chose the console
*/
int main(int argc , char * argv[])
{
    int my_rank;        /* rank of process  */
    int p;          /* number of process    */
    int source;     /* rank of sender   */
    int dest;       /* rank of reciever */
    int tag = 0;
    int i, j, k;
    int r1  ,r2 ,c1  ,c2 ;
   
    int *mat1, *mat2,*subMatrix , *resultMat , *resultRow , *RemainderRow ,* remainderResult;
    int canMult = 0;
    int subMatrixSize , resultMatrixSize , numberOfRemainderRows ;
    MPI_Status status;

    /* Start up MPI */
    MPI_Init( &argc , &argv );

    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    //  input rows and dimentions  
    if(my_rank==0){
        do {
            printf("# of rows in mat #1: ");
            scanf("%d", &r1);
            printf("# of cols in mat #1: ");
            scanf("%d", &c1);
            printf("# of rows in mat #2: ");
            scanf("%d", &r2);
            printf("# of cols in mat #2: ");
            scanf("%d", &c2);
            if (c1 == r2) {
                canMult = 1;
            } else {
                printf("those matrices Cannot be multiplied\n");
            }
        } while (canMult == 0);

        if (p>r1)
        {            
            p=r1;
            printf("\n\n new Proccess count  %d  \n\n",p);
        }

        // items per proccess 
        subMatrixSize = (r1/p) * c1;
        // Remaindr Items
        numberOfRemainderRows = (r1%p);

        // if number of proccess is larger then number of rows in mat1 , ignore them
        
    }

    // terminate extra proccesses 
    if(my_rank>=r1){
        resultRow = 0; 
        MPI_Finalize();
        return 0;
    }

    // new Proccess count
    MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //  bcast rows and colmns  
    MPI_Bcast(&r1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r2, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c2, 1, MPI_INT, 0, MPI_COMM_WORLD);


    mat1 = malloc(sizeof(int)* r1 * c1);
    mat2 = malloc(sizeof(int)* r2 * c2);
    resultMat  = malloc(sizeof(int)* r1 * c2);
    RemainderRow = malloc(sizeof(int)* c1);

    // input matrices 
    if (my_rank==0)
    {

        // input matrices 
        printf("Enter mat #1 : \n");
        for (i = 0; i < r1 * c1 ; ++i) {
            scanf("%d", &mat1[i]);
        }
        printf("Enter mat #2 : \n");
        for (i = 0; i < r2 * c2 ; ++i) {
            scanf("%d", &mat2[i]);
        }

        // Handel Remainder rows 
        for(i =0 ; i < numberOfRemainderRows ;i++){
            for ( j = 0; j < c1 ; j++){
                RemainderRow[j] = mat1[((r1*c1)-(numberOfRemainderRows+i)*c1)+j];    
                printf("REM ELEM Sender: %d - %d \n",RemainderRow[j] , r1-numberOfRemainderRows-i+j );     
            }
                
                MPI_Send(RemainderRow, c1 , MPI_INT, i+1, tag, MPI_COMM_WORLD);
        }

    }
    
    
    MPI_Bcast(&subMatrixSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numberOfRemainderRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // number of multiplication output cells 
    resultMatrixSize = c2 * (subMatrixSize/c1) ;
    //subMatrix for this proccess , 1 or more rows 
    subMatrix  = malloc(sizeof(int)* subMatrixSize );
    // multiplication outputs matrix
    resultRow  = malloc(sizeof(int)* resultMatrixSize);
    remainderResult  = malloc(sizeof(int)* c2);


    // Scatter mat1 and brodcast mat2 
    MPI_Scatter(mat1 , subMatrixSize , MPI_INT, subMatrix , subMatrixSize, MPI_INT, 0,MPI_COMM_WORLD);
    MPI_Bcast(mat2, r2 * c2, MPI_INT, 0, MPI_COMM_WORLD);
  
   
    
    //loop on subMatrix rows
    for (k = 0; k < r1/p; ++k)
    {
        // loop on each row 
        for(i=0; i< c1 ;i++){
            int temp =0 ;
            int startCol = i ;
            int startRow = c1*k ;
            for(j=0 ;j<r2 ;j++){
                temp += subMatrix[startRow+j]* mat2[startCol];
//                printf("%d - %d * %d f %d , %d , %d \n",my_rank ,subMatrix[startRow+j], mat2[startCol],i ,j ,k );
                startCol+=c2;
            }

            resultRow[i+(k*c2)]= temp ;
//          printf("%d ,%d => (%d)\n\n",my_rank,i,resultRow[i+(k*c2)] );    
        }
    }

    MPI_Gather(resultRow, resultMatrixSize , MPI_INT, resultMat,  resultMatrixSize , MPI_INT, 0,
       MPI_COMM_WORLD);


    // Handle remainder in slaves 
    if(my_rank!=0 && my_rank <= numberOfRemainderRows){
            MPI_Recv(RemainderRow, c1, MPI_INT, 0, tag, MPI_COMM_WORLD,&status);           

            // loop on the remainder row 
            for(i=0; i< c1 ;i++){
                int temp =0 ;
                int startCol = i ;

                for(j=0 ;j<r2 ;j++){
                    temp += RemainderRow[j]* mat2[startCol];
                //    printf("%d - %d * %d f %d , %d \n",my_rank ,RemainderRow[j], mat2[startCol], i ,j);
                    startCol+=r1;
                }

                remainderResult[i]= temp ;
            }
          
            MPI_Send(remainderResult, c1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }


    if (my_rank==0)
    {
        // recv remainder elements from slaves 
         for (i = 0 ; i < numberOfRemainderRows; i++) {
            MPI_Recv(RemainderRow, c1, MPI_INT, i+1, tag, MPI_COMM_WORLD, &status);

            for ( j = 0; j < c1 ; j++){

                resultMat[((r1*c1)-(numberOfRemainderRows-i)*c1)+j] = RemainderRow[j] ;                    
                printf("REM ELEM reciever : %d - %d ,%d ,%d ,%d \n",RemainderRow[j] , i ,numberOfRemainderRows,j,c1 );     
            }
                
        }


        // print result 
        printf("-------------result---------------\n");
        for (i = 0; i < r1*c2 ; ++i) {
             if(i%r1==0){
                printf("\n");
            }
            printf("%d\t" ,resultMat[i]);
           
        }
            
    }


    MPI_Finalize();

    return 0;
}
