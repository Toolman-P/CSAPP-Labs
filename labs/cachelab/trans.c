/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
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
  int i,j;
  int ii,jj;
  int c[16];
  for(i=0;i<N-(N%4!=0)*4;i+=4)
    for(j=0;j<M-(M%4!=0)*4;j+=4){

        c[0] = A[i][j];
        c[1] = A[i][j+1];
        c[2] = A[i][j+2];
        c[3] = A[i][j+3];
        
        c[4] = A[i+1][j];
        c[5] = A[i+1][j+1];
        c[6] = A[i+1][j+2];
        c[7] = A[i+1][j+3];
        
        c[8] = A[i+2][j];
        c[9] = A[i+2][j+1];
        c[10] = A[i+2][j+2];
        c[11] = A[i+2][j+3];


        c[12] = A[i+3][j];
        c[13] = A[i+3][j+1];
        c[14] = A[i+3][j+2];
        c[15] = A[i+3][j+3];

        B[j][i] = c[0];
        B[j][i+1] = c[4];
        B[j][i+2] = c[8];
        B[j][i+3] = c[12];

        B[j+1][i] = c[1];
        B[j+1][i+1] = c[5];
        B[j+1][i+2] = c[9];
        B[j+1][i+3] = c[13];

        B[j+2][i] = c[2];
        B[j+2][i+1] = c[6];
        B[j+2][i+2] = c[10];
        B[j+2][i+3] = c[14];

        B[j+3][i] = c[3];
        B[j+3][i+1] = c[7];
        B[j+3][i+2] = c[11];
        B[j+3][i+3] = c[15];

    }
  

  if(M%4!=0 || N%4!=0){
    ii=i;jj=j;
    for(i=ii;i<N;i++)
      for(j=0;j<M;j++)
        B[j][i]=A[i][j];
    for(i=0;i<N;i++)
      for(j=jj;j<M;j++)
        B[j][i]=A[i][j];
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

