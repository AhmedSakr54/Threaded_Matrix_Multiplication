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

int *** read_matrices_from_file(int * row1, int * col1, int * row2, int * col2) {
    int *** matrix_3d = (int ***) malloc(sizeof(int **));
    FILE * fp;
    fp = fopen("input-matrix.txt", "r");
    if (fp == NULL) {
        perror("Error while openeing the file.\n");
        exit(EXIT_FAILURE);
    }
    int temp_row1, temp_col1;
    fscanf(fp, "%d %d", &temp_row1, &temp_col1);
    int i, j;
    int **matrix1 = allocate_matrix(temp_row1, temp_col1);
    for (i = 0; i < temp_row1; i++) {
        for (j = 0; j < temp_col1; j++) {
            fscanf(fp, "%d", &matrix1[i][j]);
        }
    }
    matrix_3d[0] = matrix1;
    int temp_row2, temp_col2;
    fscanf(fp, "%d %d", &temp_row2, &temp_col2);
    int **matrix2 = allocate_matrix(temp_row2, temp_col2);
    for (i = 0; i < temp_row2; i++) {
        for (j = 0; j < temp_col2; j++) {
            fscanf(fp,"%d", &matrix2[i][j]);
        }
    }
    matrix_3d[1] = matrix2;
    fclose(fp);
    *row1 = temp_row1;
    *col1 = temp_col1;
    *row2 = temp_row2;
    *col2 = temp_col2;
    return matrix_3d;
}
int ** get_matrix(int *** matrix_3d, int num) {
    if (num < 0 || num >1) {
        printf("error in the argument of the function\nnum must be 0 or 1");
        return NULL;
    }
    return matrix_3d[num];
}

int main() {

    int row1, col1, row2, col2;
    FILE * fp;
    fp = fopen("input-matrix.txt", "r");
    int **matrix1 = read_matrix_from_file(fp, &row1, &col1);
    int **matrix2 = read_matrix_from_file(fp, &row2, &col2);
    // int ***matrix_3d = read_matrices_from_file(&row1, &col1, &row2, &col2);
    // int ** matrix1 = get_matrix(matrix_3d, 0);
    // int ** matrix2 = get_matrix(matrix_3d, 1);
    print_matrix(matrix1, row1, col1);
    print_matrix(matrix2, row2, col2);
    // printf("Enter the size of the 2d array: ");
    // scanf("%d %d", &row, &col);
    // int ** matrix = create_matrix(row, col);

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