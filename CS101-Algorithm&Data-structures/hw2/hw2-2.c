// Please add your code for question 2 of homework 2 to this file

// Add missing header files here
// ...

// Implement the following prototypes. The functions for the matrix operations are kept generic (i.e. the matrix size
// is kept variable through the MACRO called SIZE). Though the size is kept at 3 for this homework, please make sure
// that the functions should remain functional even in the case where SIZE differs from 3.
#include <stdio.h>
#define SIZE 3
void transpose(int ar[SIZE][SIZE]);
void swap2Rows(int ar[SIZE][SIZE], int r1, int r2);
void swap2Cols(int ar[SIZE][SIZE], int c1, int c2);
void display(int ar[SIZE][SIZE]);

// Main function that reads in a 3x3 matrix and performs the outlined transposition and swapping
// operations on it (cumulatively). Please refer to the test cases to see example runs.

void transpose(int ar[SIZE][SIZE]){
    int new_array[SIZE][SIZE] = {0,0,0,0,0,0,0,0,0};
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            new_array[j][i] = ar[i][j];
        }
    }
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            ar[i][j] = new_array[i][j];
        }
    }
}


void swap2Rows(int ar[SIZE][SIZE], int r1, int r2){
    for (int i = 0; i < SIZE; i++){
        int temp = ar[r1][i];
        ar[r1][i] = ar[r2][i];
        ar[r2][i] = temp;
    }
}

void swap2Cols(int ar[SIZE][SIZE], int c1, int c2){
    for (int i = 0; i < SIZE; i++){
        int temp = ar[i][c1];
        ar[i][c1] = ar[i][c2];
        ar[i][c2] = temp;
    }
}

void display(int ar[SIZE][SIZE]){
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            printf("%6d",ar[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Main function that reads in a 3x3 matrix and performs the outlined transposition and swapping
// operations on it (cumulatively). Please refer to the test cases to see example runs.
int main()
{
    int array[SIZE][SIZE] = {0,0,0,0,0,0,0,0,0};
    int r1 = 1;
    int c1 = 1;
    int r2 = 2;
    int c2 = 2;
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            scanf("%d",&array[i][j]);
        }
    }
    transpose(array);
    display(array);
    swap2Rows(array, r1, r2);
    display(array);
    swap2Cols(array, c1, c2);
    display(array);
    return 0;
}
