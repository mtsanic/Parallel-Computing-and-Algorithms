#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define mpitype MPI_DOUBLE

typedef double dtype;
const int size_row = 5;
const int size_col = 4;


int read_row_matrix_vector(dtype *a[size_row], dtype *b, int id, int interval) {
    int i, j;
    dtype _;

    FILE *fp = fopen("mv.txt", "r");
    if (fp == NULL) {
        printf("Open file error.\n");
        return -1;
    }
    for (i = 0; i < size_col; i++) {
        if (i >= id * interval && i < (id + 1) * interval) {
            fscanf(fp, "%lf", &b[i % interval]);
        } else {
            fscanf(fp, "%lf", &_);
        }
    }

    for (i = 0; i < size_row; i++) {
        for (j = 0; j < size_col; j++) {
            if (j >= id * interval && j < (id + 1) * interval) {
                fscanf(fp, "%lf", &a[i][j % interval]);
            } else {
                fscanf(fp, "%lf", &_);
            }
        }
    }
    fclose(fp);

    // printf("Matrix A on process %d:\n", id);
    // for (i = 0; i < size_row; i++) {
    //     for (j = 0; j < interval; j++) { printf("  %f ", a[i][j]); }
    //     printf("\n");
    // }
    // printf("\n");
    //
    // printf("Vector B on process %d:\n", id);
    // for (j = 0; j < interval; j++) { printf("    %f\n", b[j]); }
    // printf("\n");

    return 1;
}


int main(int argc, char *argv[]) {
    int id, np, root = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int interval = (size_col + np - 1) / np, i, j;
    if ((id + 1) * interval > size_col) interval = size_col - id * interval;

    dtype *a[size_row], *b, c[size_row], c_global[size_row];
    if (interval > 0) {
        b = (dtype *) malloc(interval * sizeof(dtype));
        for (i = 0; i < size_row; i++) { a[i] = (dtype *) malloc(interval * sizeof(dtype)); }

        //read matrix and vector from file
        read_row_matrix_vector(a, b, id, interval);
    }

    //compute result
    printf("Process %d responsible for %d columns.\n", id, interval);
    for (i = 0; i < size_row; i++) {
        c[i] = 0.0;
        for (j = 0; j < interval; j++) c[i] += a[i][j] * b[j];
    }

    //reduce and print the result
    MPI_Reduce(c, c_global, size_row, mpitype, MPI_SUM, root, MPI_COMM_WORLD);
    if (id == root) {
        printf("Result vector C:\n");
        for (i = 0; i < size_row; i++) { printf("    %f\n", c_global[i]); }
    }

    if (interval > 0) {
        free(b);
        for (i = 0; i < size_row; i++) { free(a[i]); }
    }
    MPI_Finalize();
    return 0;
}
