#pragma once
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

enum Type {
    MATRIX,
    VECTOR
};

double* read_matrix(uint32_t *rows, uint32_t *cols, enum Type n, char*arg) {
    char*path = (char*)malloc(sizeof(char) * (13 + strlen(arg)));

    if (n == MATRIX)
        strcpy(path, "data/matrix_");
    else
        strcpy(path, "data/vector_");

    path[12] = 0;
    strcat(path, arg);
    path[12+strlen(arg)] = 0;

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("there is no such configuration");
        free(path);
        return NULL;
//        int status = 0;
//        MPI_Finalize();
//        exit(status);
    }
    int cnt;
    cnt = fread(rows, sizeof(uint32_t), 1, file);
    cnt = fread(cols, sizeof(uint32_t), 1, file);
    if (!cnt) {
        perror("err while reading matrix");
        exit(1);
    }
//    printf("rows: %d\n", *rows);
//    printf("cols: %d\n", *cols);

    uint32_t elems = *rows * *cols;
    double *values = (double*)malloc(elems * sizeof(double));
    int i;
    for (i = 0; i < elems; i++) {
        cnt = fread(&values[i], sizeof(double), 1, file);
//        printf("val[%d] = %f\n", i, values[i]);
    }

    fclose(file);
    free(path);
    return values;
}
