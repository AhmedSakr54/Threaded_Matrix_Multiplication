#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct Matrix {
    int ** array;
    int row;
    int col;
}Matrix;

typedef struct Thread_data {
    int * matrix_row_elements;
    int * matrix_col_elements;
    int num_of_multiplications;
}Thread_data;

void *mult_to_get_one_output_element(void *thread_arg) {
    Thread_data *data;
    data = (Thread_data *) thread_arg;

    int resultant = 0;
    int i;
    for (i = 0; i < data->num_of_multiplications; i++) {
        resultant += data->matrix_row_elements[i] * data->matrix_col_elements[i];
    }
    int *returned_resultant = (int*) malloc(sizeof(int));

    *returned_resultant = resultant;
    pthread_exit(returned_resultant);
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

void write_to_matrix_file(Matrix matrix, float time, int num) {
    FILE *fp;
    fp = fopen("output-matrix.txt", "a+");
    int i, j;
    for (i = 0; i < matrix.row; i++) {
        for (j = 0; j < matrix.col; j++) {
            fprintf(fp, "%d ", matrix.array[i][j]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "END%d\t %f ms", num, time);
    fclose(fp);
}

Matrix thread_per_output_element_matmul(Matrix matrix1, Matrix matrix2) {
    if (matrix1.col != matrix2.row) {
        printf("Inconsistent dimensions of the matrices\n");
        exit(1);
    }
    Matrix mult_matrix;
    mult_matrix.row = matrix1.row;
    mult_matrix.col = matrix2.col;
    mult_matrix.array = allocate_Matrix_array(mult_matrix.row, mult_matrix.col);
    
    int num_threads = mult_matrix.row * mult_matrix.col;
    pthread_t *threads;
    threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));

    int count = 0;
    Thread_data data[num_threads];
    int i, j, k;
    for (i = 0; i < mult_matrix.row; i++) {
        for (j = 0; j < mult_matrix.col; j++) {
            data[count].matrix_row_elements = (int *) malloc(mult_matrix.row*sizeof(int));
            data[count].matrix_col_elements = (int *) malloc(mult_matrix.col*sizeof(int));
            data[count].num_of_multiplications = matrix1.col;

            for (k = 0; k < matrix1.col; k++)
                data[count].matrix_row_elements[k] = matrix1.array[i][k];

            for (k = 0; k < matrix2.row; k++)
                data[count].matrix_col_elements[k] = matrix2.array[k][j];

            pthread_create(&threads[count], NULL, mult_to_get_one_output_element, (void*) &data[count]);
            count++;
        }
    }
    count = 0;
    for (i = 0; i < mult_matrix.row; i++) {
        for (j = 0; j < mult_matrix.col; j++) {
            void *thread_ret_val;
            pthread_join(threads[count++], &thread_ret_val);
            int * temp = (int *) thread_ret_val;
            mult_matrix.array[i][j] = *temp;
        }
    }
    return mult_matrix;
}

int main() {

    FILE * fp;
    fp = fopen("input-matrix.txt", "r");
    Matrix matrix1 = read_matrix_from_file(fp);
    Matrix matrix2 = read_matrix_from_file(fp);
    fclose(fp);
    print_matrix(matrix1);
    print_matrix(matrix2);

    clock_t start_t, end_t;
    start_t = clock();
    Matrix matrix3 = thread_per_output_element_matmul(matrix1, matrix2);
    end_t = clock();

    print_matrix(matrix3);
    float total_time = (float)(end_t - start_t) / CLOCKS_PER_SEC * 1000;
    write_to_matrix_file(matrix3, total_time, 1);

    return 0;
}