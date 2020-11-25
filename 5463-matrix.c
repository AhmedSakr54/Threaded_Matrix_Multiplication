#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *mult_two_numbers(void *thread_arg) {
    int * matrix_nums = (int *) thread_arg;
    printf("%d\n" ,matrix_nums[0]*matrix_nums[1]);
    pthread_exit(NULL);
}

void print_matrix(int** array, int n, int m) {
    int i, j;
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            printf("%d ", array[i][j]);
        }
        printf("\n");
    }
    printf("===================\n");
}

int ** allocate_matrix(int n, int m) {
    int i,j;
    int ** returned_matrix = (int **) malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        returned_matrix[i] = (int *) malloc(m * sizeof(int));
    }
    return returned_matrix;
}

int ** read_matrix_from_file(FILE *fp, int *row, int *col) {
    int temp_row, temp_col;
    fscanf(fp, "%d %d", &temp_row, &temp_col);
    int i, j;
    int **matrix = allocate_matrix(temp_row, temp_col);
    for (i = 0; i < temp_row; i++) {
        for (j = 0; j < temp_col; j++) {
            fscanf(fp, "%d", &matrix[i][j]);
        }
    }
    *row = temp_row;
    *col = temp_col;
    return matrix;
}

int main() {

    int row1, col1, row2, col2;
    FILE * fp;
    fp = fopen("input-matrix.txt", "r");
    int **matrix1 = read_matrix_from_file(fp, &row1, &col1);
    int **matrix2 = read_matrix_from_file(fp, &row2, &col2);
    print_matrix(matrix1, row1, col1);
    print_matrix(matrix2, row2, col2);

    // int num_threads = row * col;
    // pthread_t threads[num_threads];
    // int *taskids[num_threads];
    // int rc,i;
    // for (i = 0; i < num_threads; i++) {
    //     printf("Creating thread %d\n", i);
    //     rc = pthread_create(&threads[i], NULL, mult_two_numbers, (void *) &arr);
    //     if (rc) {
    //         printf("ERROR; return code from pthread_create() is %d\n", rc);
    //         exit(-1);
    //     }
    // }
    // pthread_exit(NULL);
    return 0;
}