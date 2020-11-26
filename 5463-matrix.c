#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct Matrix {
    int ** array;
    int row;
    int col;
}Matrix;

void destory_matrix(Matrix matrix) {
    int i;
    for (i = 0; i < matrix.row; i++) {
        free(matrix.array[i]);
    }
    free(matrix.array);
}

typedef struct Thread_data {
    int * matrix_row_elements;
    int * matrix_col_elements;
    int num_of_multiplications;
}Thread_data;

typedef struct Theard_whole_row_data {
    int * array1_row_elements;
    int ** array2;
    int num_rows;
    int num_cols;
}Theard_whole_row_data;

void check_dimensions(int row, int col) {
    if (row != col) {
        printf("Inconsistent dimensions of the matrices\n");
        exit(1);
    }
}

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

void *mult_to_get_row_output_elements(void *thread_arg) {
    Theard_whole_row_data *data;
    data = (Theard_whole_row_data *) thread_arg;
    int * row_results = (int *) malloc(data->num_cols*sizeof(int));
    int i, j;
    for (i = 0; i < data->num_rows; i++) {
        row_results[i] = 0;
        for (j = 0; j < data->num_cols; j++) {
            row_results[i] += data->array1_row_elements[j] * data->array2[j][i]; 
        }
    }
    pthread_exit(row_results);
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

void write_to_matrix_file(Matrix matrix, float time, int num, FILE * fp) {
    int i, j;
    for (i = 0; i < matrix.row; i++) {
        for (j = 0; j < matrix.col; j++) {
            fprintf(fp, "%d ", matrix.array[i][j]);
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "END%d\t %f ms\n", num, time);
}

Matrix initialize_mult_matrix(Matrix matrix1, Matrix matrix2) {
    Matrix mult_matrix;
    mult_matrix.row = matrix1.row;
    mult_matrix.col = matrix2.col;
    mult_matrix.array = allocate_Matrix_array(mult_matrix.row, mult_matrix.col);
    return mult_matrix;
}

Matrix thread_per_output_element_matmul(Matrix matrix1, Matrix matrix2) {
    check_dimensions(matrix1.col, matrix2.row);
    Matrix mult_matrix = initialize_mult_matrix(matrix1, matrix2);
    int num_threads = mult_matrix.row * mult_matrix.col;
    pthread_t *threads;
    threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
    int count = 0;
    Thread_data *data = (Thread_data *) malloc(num_threads * sizeof(Thread_data));
    int i, j, k;
    for (i = 0; i < mult_matrix.row; i++) {
        for (j = 0; j < mult_matrix.col; j++) {
            data[count].matrix_row_elements = matrix1.array[i];
            data[count].matrix_col_elements = (int *) malloc(mult_matrix.col*sizeof(int));
            data[count].num_of_multiplications = matrix1.col;
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
    free(threads);
    free(data);
    return mult_matrix;
}

Matrix thread_pre_output_row_matmul(Matrix matrix1, Matrix matrix2) {
    check_dimensions(matrix1.col, matrix2.row);
    Matrix mult_matrix = initialize_mult_matrix(matrix1, matrix2);
    int num_threads = mult_matrix.row;
    pthread_t *threads;
    threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
    int count = 0;
    Theard_whole_row_data *data = (Theard_whole_row_data *) malloc(num_threads * sizeof(Theard_whole_row_data));
    int i, j, k;
    for (i = 0; i < mult_matrix.row; i++) {
        data[count].array1_row_elements = matrix1.array[i];
        data[count].array2 = matrix2.array;
        data[count].num_rows = matrix2.col;
        data[count].num_cols = matrix1.col;
        pthread_create(&threads[count], NULL, mult_to_get_row_output_elements, (void*) &data[count]);
        count++;
    }
    count = 0;
    for (i = 0; i < mult_matrix.row; i++) {
        void *thread_ret_val;
        pthread_join(threads[count++], &thread_ret_val);
        int * temp = (int *) thread_ret_val;
        mult_matrix.array[i] = temp;
    }
    free(threads);
    free(data);
    return mult_matrix;
}

float get_elapsed_time(clock_t start, clock_t end) {
    return (float) (end - start) / CLOCKS_PER_SEC * 1000;
}

void test_threading_techniques_in_matmul() {
    FILE * fp;
    clock_t start_t, end_t;
    float total_time;

    fp = fopen("input-matrix.txt", "r");
    Matrix matrix1 = read_matrix_from_file(fp);
    Matrix matrix2 = read_matrix_from_file(fp);
    fclose(fp);

    fp = fopen("output-matrix.txt", "w");

    start_t = clock();
    Matrix matrix3 = thread_per_output_element_matmul(matrix1, matrix2);
    end_t = clock();
    total_time = get_elapsed_time(start_t, end_t);
    write_to_matrix_file(matrix3, total_time, 1, fp);
    destory_matrix(matrix3);

    start_t = clock();
    Matrix matrix4 = thread_pre_output_row_matmul(matrix1, matrix2);
    end_t = clock();
    total_time = get_elapsed_time(start_t, end_t);
    write_to_matrix_file(matrix4, total_time, 2, fp);

    fclose(fp);

    destory_matrix(matrix4);
    destory_matrix(matrix1);
    destory_matrix(matrix2);
    pthread_exit(NULL);
}

int main() {

    test_threading_techniques_in_matmul();

    return 0;
}