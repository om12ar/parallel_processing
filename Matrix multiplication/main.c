#include <stdio.h>
#include <stdlib.h>
// Omar Ibrahim Saber
// 20130316
// CS4
int main(void) {
	int ch;
	int i, j, k;
	int **mat1, **mat2, **res;

	do {
		printf("Enter 1 to read from file or 2 for console input : \n");
		scanf("%d", &ch);
	} while (ch != 1 && ch != 2);

	int r1, r2, c1, c2;
	int canMult = 0;
/////////// Files /////////////////
	if (ch == 1) {

		FILE *file = fopen("in.txt", "r");

		if (file == NULL) {
			printf("Error! Could not open file\n");
			exit(-1);
		}

		int nums[1024];
		int fsize = 0;
		while (fscanf(file, "%d", &nums[fsize]) != EOF) {
		//	printf("Number %d is: %d\n", fsize, nums[fsize]);
			fsize++;
		}

		r1 = nums[0];
		c1 = nums[1];
		r2 = nums[2];
		c2 = nums[3];
        if (c1 != r2) {
            printf("those matrices Cannot be multiplied\n");
            exit(-1);
        }

		mat1 = malloc(sizeof(int*) * r1);
		mat2 = malloc(sizeof(int*) * r2);
		res = malloc(sizeof(int*) * r1);
		for (i = 0; i < r1; i++) {
			mat1[i] = (int *) malloc(sizeof(int) * c1);
			res[i] = (int *) malloc(sizeof(int) * c2);
		}
		for (i = 0; i < r2; i++) {
			mat2[i] = (int *) malloc(sizeof(int) * c2);
		}

		int putInArray = 4;
		for (i = 0; i < r1; ++i) {
			for (j = 0; j < c1; ++j) {
				mat1[i][j] = nums[putInArray++];
			}
		}
		for (i = 0; i < r2; ++i) {
			for (j = 0; j < c2; ++j) {
				mat2[i][j] = nums[putInArray++];
			}
		}


}
/////////// Console /////////////////
	else {
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

		mat1 = malloc(sizeof(int*) * r1);
		mat2 = malloc(sizeof(int*) * r2);
		res = malloc(sizeof(int*) * r1);
		for (i = 0; i < r1; i++) {
			mat1[i] = (int *) malloc(sizeof(int) * c1);
			res[i] = (int *) malloc(sizeof(int) * c2);
		}
		for (i = 0; i < r2; i++) {
			mat2[i] = (int *) malloc(sizeof(int) * c2);
		}

		printf("Enter mat #1 : \n");
		for (i = 0; i < r1; ++i) {
			for (j = 0; j < c1; ++j) {
				scanf("%d", &mat1[i][j]);
			}
		}
		printf("Enter mat #2 : \n");
		for (i = 0; i < r2; ++i) {
			for (j = 0; j < c2; ++j) {
				scanf("%d", &mat2[i][j]);
			}
		}
	}

	for (i = 0; i < r1; ++i) {

		for (j = 0; j < c2; ++j) {
			int cell = 0;
			for (k = 0; k < c1; k++) {
				cell += mat1[i][k] * mat2[k][j];
			}
			res[i][j] = cell;
		}

	}
	printf("Result Matrix is ( %d x %d )  : \n" , r1 , c2);
	for (i = 0; i < r1; ++i) {
		for (j = 0; j < c2; ++j) {
			printf("%d \t", res[i][j]);
		}
		printf("\n");
	}


	// Deacllocation

		for (i = 0; i < r1; i++) {
			free(mat1[i]) ;
			free(res[i]) ;
		}
		for (i = 0; i < r2; i++) {
			free(mat2[i]);
		}
		free (mat1) ;
		free (mat2);
		free (res);
	return EXIT_SUCCESS;
}

