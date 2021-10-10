#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>

double timer() {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

typedef struct dense dense;

struct dense {
    int32_t nrows;
    int32_t *vals;
};

void read_matrix(char *filename, dense *M) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Can't open matrix file");
    }

    char type;

    int cnt;
    cnt = fread(&type, sizeof(char), 1, file);
    cnt = fread(&M->nrows, sizeof(uint32_t), 1, file);
    if (!cnt) {
        perror("err while reading matrix");
        exit(1);
    }
    printf("rows: %d\n", M->nrows);

    uint32_t elems = M->nrows * M->nrows;
    M->vals = (int32_t *)malloc(elems * sizeof(int32_t));
    uint32_t i;
    for (i = 0; i < elems; i++) {
        cnt = fread(&M->vals[i], sizeof(int32_t), 1, file);
        printf("val[%d] = %d\n", i, M->vals[i]);
    }

    fclose(file);
}



int main (int argc, char**argv) {
    assert(argc == 5);
    char *A_name = argv[1];
    char *B_name = argv[2];
    char *C_name = argv[3];

    dense A, B, C;
    read_matrix(A_name, &A);
    read_matrix(B_name, &B);
    read_matrix(C_name, &C);

    double t1 = timer();

    double t2 = timer();

    printf("Elapsed time: %.5f\n", t2-t1);
    return 0;
}
