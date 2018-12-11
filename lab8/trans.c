/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 *
 * Student Name:wulianyi
 * Student ID:516030910252
 *
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M==32)
    {
	int temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8;
    	for (int i=0;i<32;i+=8) {
	    for (int j=0;j<32;j++) {
	    	temp1=A[j][i];
	    	temp2=A[j][i+1];
	    	temp3=A[j][i+2];
	    	temp4=A[j][i+3];
	    	temp5=A[j][i+4];
	    	temp6=A[j][i+5];
	    	temp7=A[j][i+6];
	    	temp8=A[j][i+7];
	    	B[i][j]=temp1;
	    	B[i+1][j]=temp2;
	    	B[i+2][j]=temp3;
	    	B[i+3][j]=temp4;
	    	B[i+4][j]=temp5;
	    	B[i+5][j]=temp6;
	    	B[i+6][j]=temp7;
	    	B[i+7][j]=temp8;
	    }
   	}
    }

    if (M==64)
    {
    	int temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8;

    	for (int i=0;i<64;i+=8) {
            for (int j=0;j<64;j+= 8) {
            	for (int k=j;k<j+4;k++){
        	    temp1=A[k][i];
        	    temp2=A[k][i+1];
        	    temp3=A[k][i+2];
        	    temp4=A[k][i+3];
        	    temp5=A[k][i+4];
        	    temp6=A[k][i+5];
        	    temp7=A[k][i+6];
        	    temp8=A[k][i+7];
        	    B[i][k]=temp1;
        	    B[i+1][k]=temp2;
        	    B[i+2][k]=temp3;
        	    B[i+3][k]=temp4;
        	    B[i][k+4]=temp5;
        	    B[i+1][k+4]=temp6;
        	    B[i+2][k+4]=temp7;
        	    B[i+3][k+4]=temp8;
            	}
            	for (int k=i;k<i+4;k++)
           	{
        	    temp1=B[k][j+4];
        	    temp2=B[k][j+5];
        	    temp3=B[k][j+6];
        	    temp4=B[k][j+7];
        	    temp5=A[j+4][k];
        	    temp6=A[j+5][k];
        	    temp7=A[j+6][k];
        	    temp8=A[j+7][k];
        	    B[k][j+4]=temp5;
        	    B[k][j+5]=temp6;
        	    B[k][j+6]=temp7;
        	    B[k][j+7]=temp8;
        	    temp5=A[j+4][k+4];
        	    temp6=A[j+5][k+4];
        	    temp7=A[j+6][k+4];
        	    temp8=A[j+7][k+4];
        	    B[k+4][j]=temp1;
        	    B[k+4][j+1]=temp2;
        	    B[k+4][j+2]=temp3;
        	    B[k+4][j+3]=temp4;
        	    B[k+4][j+4]=temp5;
        	    B[k+4][j+5]=temp6;
        	    B[k+4][j+6]=temp7;
        	    B[k+4][j+7]=temp8;
            	}
            }
    	}   
    }

    if (M==61)
    {
    	int temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8;

    	for (int i=0;i<61;i+=8) {
	    for (int j=0;j<67;j++) {
	        temp1=A[j][i];
	    	temp2=A[j][i+1];
	    	temp3=A[j][i+2];
	    	temp4=A[j][i+3];
	    	temp5=A[j][i+4];
	    	temp6=A[j][i+5];
	    	temp7=A[j][i+6];
	    	temp8=A[j][i+7];
	    	B[i][j]=temp1;
	    	B[i+1][j]=temp2;
	    	B[i+2][j]=temp3;
	    	B[i+3][j]=temp4;
	    	B[i+4][j]=temp5;
	    	B[i+5][j]=temp6;
	    	B[i+6][j]=temp7;
	    	B[i+7][j]=temp8;
	    }
   	} 
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}



/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

