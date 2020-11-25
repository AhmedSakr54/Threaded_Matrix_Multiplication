#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct Matrix {
    int ** array;
    int row;
    int col;
}Matrix;

void *mult_two_numbers(void *thread_arg) {
    int * matrix_nums = (int *) thread_arg;
    printf("%d\n" ,matrix_nums[0]*matrix_nums[1]);
    pthread_exit(NULL);
}

void print_matrix(Matrix mat) {
    int i, j;
    for (i = 0; i < mat.row; i++) {
        for (j = 0; j < mat.col; j++) {
            printf("%d ", mat.array[i][j]);
        }
        printf("\n");
    }
    printf("========================\n");
}

int ** allocate_Matrix_array(int n, int m) {
    int i, j;
    int ** returned_matrix = (int **) malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        returned_matrix[i] = (int *) malloc(m * sizeof(int));
    }
    return returned_matrix;
}

Matrix read_matrix_from_file(FILE *fp) {
    Matrix mat;
    fscanf(fp, "%d %d", &mat.row, &mat.col);
    int i, j;
    mat.array = allocate_Matrix_array(mat.row, mat.col);
    for (i = 0; i < mat.row; i++) {
        for (j = 0; j < mat.col; j++) {
            fscanf(fp, "%d", &mat.array[i][j]);
        }
    }
    return mat;
}

void normal_matrix_multiply(int ** matrix1, int ** matrix2, int row1, int col1, int row2, int col2) {
    if (row1 != col2) {
        printf("Inconsistent dimensions of the matrices\n");
        return;
    }
    
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
}
int main() {

    int row1, col1, row2, col2;
    FILE * fp;
    fp = fopen("input-matrix.txt", "r");
    Matrix matrix1 = read_matrix_from_file(fp);
    Matrix matrix2 = read_matrix_from_file(fp);
    fclose(fp);
    print_matrix(matrix1);
    print_matrix(matrix2);
    printf("%d %d\n", matrix1.row, matrix1.col);
    printf("%d %d\n", matrix2.row, matrix2.col);
    // normal_matrix_multiply(matrix1, matrix2, row1, col1, row2, col2);
    return 0;
}