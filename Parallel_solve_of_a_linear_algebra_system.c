#include <stdio.h>
#include <time.h>
#include <string.h>
#include "mpi.h"
#define _CRT_SECURE_NO_WARNINGS
/*
Omar Ibrahim Saber 
20130316    
CS4


1- The matrix is in this form -Assuming a 3 var system - :
  mat1 = 
  X1 Y1 Z1 A1
  X2 Y2 Z2 A2
  X3 Y3 Z3 A3

2-  the Inverse is calculated in a matrix called invMat and the values of the mat1 are replaced with the inv
notice the As vector is the same 

  mat1 after inverse = 
  InvX1   InvY1   InvZ1    A1
  InvX2   InvY2   InvZ1    A2
  InvX3   InvY3   InvZ1    A3


3-  the rows are scattered 

  P1 : InvX1   InvY1   InvZ1   A1
  -------------------------------
  P2 : InvX2   InvY2   InvZ1   A2
  -------------------------------
  P3 : InvX3   InvY3   InvZ1   A3

4- the As are gathered in each proccess using all_gather 

  P1 : InvX1   InvY1   InvZ1   A1
      subVec[A1,A2,A3]
  -------------------------------
  P2 : InvX2   InvY2   InvZ1   A2
      subVec[A1,A2,A3]
  -------------------------------
  P3 : InvX3   InvY3   InvZ1   A3
      subVec[A1,A2,A3]

5-  multiplication 
    Invx1 * A1 + ...
    .
    .
    .

6- results are gathered in P0
  P0 : Vec (Xval , Yval , Zval)




---------------------------------------------------------------

a real life example : 
like when deciding to build a new super computer there are multiple choices with different costs ,
and we are interested in lets say 3 operations : floating point operation , loading and storing and logical operations
lets call them X , Y and Z:

X : floating point operation
Y : loading and storing 
Z : logical operations

if the first computer can do them in the following speeds -units are nano seconeds- :

2 , 4 , 3 and to reach this compination of speeds it will cost 1000000 $ 

 Its equation will be :
2 X + 4 Y + 3 Z = 1000000

and lets say that the second equation with cost is is :
1 X + 3 Y + 4 Z = 2000000

and the third is :
1 X + 2 Y + 3 Z = 3000000

we will need to solve the linear equations to get to the X , Y and Z that satisfy this performance / cost function

*/
void CoFactor(double *a,int n,double *b);

int main(int argc , char * argv[])
{
    int my_rank;        /* rank of process  */
    int p;          /* number of process    */
    int source;     /* rank of sender   */
    int dest;       /* rank of reciever */
    int tag = 0;
    int i, j, k;
    int r1 ,c1 ;
    double *mat1 ,*subMatrix , *resultMat , *resultRow  , * invMat , * vec;
    int canMult = 0;
    int subMatrixSize , resultMatrixSize  ;
    MPI_Status status;

    /* Start up MPI */
    MPI_Init( &argc , &argv );

    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    //  input rows and dimentions  
    if(my_rank==0){
        printf("# of variables : ");
        scanf("%d", &r1);
        c1 = r1 +1 ;

        if (p>r1)
        {            
            p=r1;
            printf("\n\n new Proccess count  %d  \n\n",p);
        }

        // items per proccess 
        subMatrixSize = (r1/p) * c1;
        
    }

  /*  // terminate extra proccesses 
    if(my_rank>=r1){
        resultRow = 0; 
        MPI_Finalize();
        return 0;
    }
*/
    // new Proccess count
    MPI_Bcast(&p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //  bcast rows and colmns  
    MPI_Bcast(&r1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c1, 1, MPI_INT, 0, MPI_COMM_WORLD);



    mat1 = malloc(sizeof(double)* r1 * c1);
    resultMat  = malloc(sizeof(double)* r1 );
    invMat  = malloc(sizeof(double)* r1 * r1);

    // input matrices 
    if (my_rank==0)
    {

        // input matrices 
        printf("enter the values of each equation followed by the right hand side :\n ex : 3x + 4Y + 2Z = 10 should be enterd as '3 4 2 10'  \n");
        for (i = 0; i < r1 * c1 ; ++i) {
            scanf("%lf", &mat1[i]);
        }
   
        double * temp = malloc(sizeof(double)*r1*r1);
        j =0;
        for(i =1 ; i <= r1 * c1 ; i++ ){
            if (i!=0 && i%(c1) ==0 )
            {
                continue;
            }
            temp[j++] = mat1[i-1];
        }
    

        CoFactor(temp , r1 , invMat);

        printf("Inverse MAtrix: \n");
        for ( i = 0; i < r1* r1 ; ++i)
        {   
            if(i%r1==0)
                printf("\n");
            printf("%lf  ", invMat[i]);
        }
        printf("\n");
        j=0 ;
        for(i =1 ; i <= r1 * c1 ; i++ ){
            if ( i%(c1) ==0 )
            {
                continue;
            }
            mat1[i-1] = invMat[j++];
          
        }
       

    }

    MPI_Bcast(&subMatrixSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // number of multiplication output cells 
    resultMatrixSize = subMatrixSize/c1 ;
    //subMatrix for this proccess , 1 or more rows 
    subMatrix  = malloc(sizeof(double)* subMatrixSize );
    // multiplication outputs matrix
    resultRow  = malloc(sizeof(double)* resultMatrixSize);
    vec  = malloc(sizeof(double) * r1 );



    MPI_Scatter(mat1 , subMatrixSize , MPI_DOUBLE, subMatrix , subMatrixSize, MPI_DOUBLE, 0,MPI_COMM_WORLD);

    
  int rowsEach = r1/p ;
    double * subVec = malloc(sizeof(double)* (rowsEach));
  
    for (k = 0; k < rowsEach; ++k)
    {        

        subVec[k] = subMatrix[(k*c1)+r1];
         

    }
    MPI_Allgather(subVec,rowsEach,MPI_DOUBLE,vec,rowsEach,MPI_DOUBLE,MPI_COMM_WORLD);

 
    //loop on subMatrix rows
    for (k = 0; k < rowsEach; ++k)
    {
        double temp =0 ;
        int startRow = c1*k ;
        for(i=0; i< r1 ;i++){
            temp += subMatrix[startRow+i]* vec[i];
        }
        resultRow[(k*c1)]= temp ;
    }
    MPI_Gather(resultRow, resultMatrixSize , MPI_DOUBLE, resultMat,  resultMatrixSize , MPI_DOUBLE, 0,
       MPI_COMM_WORLD);


    if (my_rank==0)
    {


        // print result 
        printf("-------------result---------------\n");
        for (i = 0; i < r1 ; ++i) {
             if(i%r1==0){
                printf("\n");
            }
            printf("%lf\t" ,resultMat[i]);
           
        }
            printf("\n");
    }



    MPI_Finalize();

    return 0;
}




int Mpow(int base, int exp)
    {
      if(exp < 0)
        return -1;

        int result = 1;
        while (exp)
        {
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        }

        return result;
    }



/*
   Recursive definition of determinate using expansion by minors.
*/
double Determinant(double **a,int n)
{
   int i,j,j1,j2;
   double det = 0;
   double **m = NULL;

   if (n < 1) { /* Error */
        return 1.0;
   } else if (n == 1) { /* Shouldn't get used */
      det = a[0][0];
   } else if (n == 2) {
      det = a[0][0] * a[1][1] - a[1][0] * a[0][1];
   } else {
      det = 0;
      for (j1=0;j1<n;j1++) {
         m = malloc((n-1)*sizeof(double *));
         for (i=0;i<n-1;i++)
            m[i] = malloc((n-1)*sizeof(double));
         for (i=1;i<n;i++) {
            j2 = 0;
            for (j=0;j<n;j++) {
               if (j == j1)
                  continue;
               m[i-1][j2] = a[i][j];
               j2++;
            }
         }
         det += Mpow(-1.0,j1+2.0) * a[0][j1] * Determinant(m,n-1);
        
      }
   }
   return(det);
}

/*
   Find the cofactor matrix of a square matrix
*/
void CoFactor(double *a,int n,double *b)
{
   int i,j,ii,jj,i1,j1;
   double det;
   double **c;

   c = malloc((n-1)*sizeof(double *));
   for (i=0;i<n-1;i++)
     c[i] = malloc((n-1)*sizeof(double));

   for (j=0;j<n;j++) {
      for (i=0;i<n;i++) {

         /* Form the adjoint a_ij */
         i1 = 0;
         for (ii=0;ii<n;ii++) {
            if (ii == i)
               continue;
            j1 = 0;
            for (jj=0;jj<n;jj++) {
               if (jj == j)
                  continue;
               //c[i1][j1] = a[ii][jj];
              c[i1][j1] = a[(ii*n) + jj ];
               j1++;
            }
            i1++;
         }

         /* Calculate the determinate */
         det = Determinant(c,n-1);

         /* Fill in the elements of the cofactor */
         b[(j*n)+i] = Mpow(-1.0,i+j+2.0) * det;
      }
   }
   double matDet = 0.0;
   double **ori2d;
   

   ori2d = malloc((n)*sizeof(double *));
   for (i=0;i<n;i++)
     ori2d[i] = malloc((n)*sizeof(double));
    

   for(i =0 ;i < n ; i++){
       for(j=0 ;j<n ;j++){
            ori2d[i][j] = a[(i*n)+j];
       }
   }
   matDet = Determinant(ori2d,n);
   for(i =0 ;i <  n*n ; i++){
        b[i] /=matDet ;

   }

  
}

/*
   Transpose of a square matrix, do it in place
*/
void Transpose(double **a,int n)
{
   int i,j;
   double tmp;

   for (i=1;i<n;i++) {
      for (j=0;j<i;j++) {
         tmp = a[i][j];
         a[i][j] = a[j][i];
         a[j][i] = tmp;
      }
   }
}