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

enum multiply { IJK, IKJ, KIJ, JIK, JKI, KJI };

typedef struct dense dense;

struct dense {
    int32_t nrows;
    int32_t *vals;
    int64_t *big_vals;
    char type;
};

void read_matrix (char *filename, dense *M) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Can't open matrix file");
    }

    int64_t cnt = 0;
    cnt += fread(&M->type, sizeof(char), 1, file);
    cnt += fread(&M->nrows, sizeof(int32_t), 1, file);
    assert(cnt == 2);
    int64_t i;
    int64_t elems = M->nrows * M->nrows;
    cnt = 0;
    if (M->type == 'd') {
        M->vals = (int32_t *)malloc(elems * sizeof(int32_t));
        for (i = 0; i < elems; i++) {
            cnt += fread(&M->vals[i], sizeof(int32_t), 1, file);
        }
    } else if (M->type == 'l') {
        M->big_vals = (int64_t *)malloc(elems * sizeof(int32_t));
        for (i = 0; i < elems; i++) {
            cnt += fread(&M->big_vals[i], sizeof(int64_t), 1, file);
        }
    } else {
        printf("Incorrect data type\n");
    }
    assert(cnt == elems);
    fclose(file);
}

void write_matrix (char *filename, dense *M) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("cant open file");
    }
    int64_t cnt = 0;
    cnt += fwrite(&M->type, sizeof(char), 1, file);
    cnt += fwrite(&M->nrows, sizeof(int32_t), 1, file);
    assert(cnt == 2);
    int64_t i;
    int64_t elems = M->nrows * M->nrows;
    cnt = 0;
    if (M->type == 'd') {
        for (i = 0; i < elems; i++) {
            cnt += fwrite(&M->vals[i], sizeof(int32_t), 1, file);
        }
    } else if (M->type == 'l') {
        for (i = 0; i < elems; i++) {
            cnt += fwrite(&M->big_vals[i], sizeof(int64_t), 1, file);
        }
    } else {
        printf("Incorrect data type\n");
    }
    assert(cnt == elems);
    fclose(file);
}

void print (dense *M) {
    int32_t nrows = M->nrows, i,j;
    printf("Matrix size : %d\n", nrows);
    printf("Matrix elems type: %c\n", M->type);
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < nrows; j++) {
            printf("%d ", M->vals[i*nrows+j]);
        }
        printf("\n");
    }
}

void ijk_dgemm (dense *A, dense *B, dense *C) {
    int32_t i,j,k;
    int32_t nrows = A->nrows;
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < nrows; j++) {
            int64_t sum = 0;
            for (k = 0; k < nrows; k++)
                sum += A->vals[i * nrows + k] * B->vals[k * nrows + j];
            C->vals[i * nrows + j] = sum;
        }
    }
}

void ikj_dgemm (dense *A, dense *B, dense *C) {
    int32_t i,j,k;
    int32_t nrows = A->nrows;
    for (i = 0; i < nrows; i++) {
        for (k = 0; k < nrows; k++) {
            int64_t r = A->vals[i * nrows + k];
            for (j = 0; j < nrows; j++)
                C->vals[i * nrows + j] += r * B->vals[k * nrows + j];
        }
    }
}

void kij_dgemm (dense *A, dense *B, dense *C) {
    int32_t i,j,k;
    int32_t nrows = A->nrows;
    for (k = 0; k < nrows; k++) {
        for (i = 0; i < nrows; i++) {
            int64_t r = A->vals[i * nrows + k];
            for (j = 0; j < nrows; j++)
                C->vals[i * nrows + j] += r * B->vals[k * nrows + j];
        }
    }
}

void jik_dgemm (dense *A, dense *B, dense *C) {
    int32_t i,j,k;
    int32_t nrows = A->nrows;
    for (j = 0; j < nrows; j++) {
        for (i = 0; i < nrows; i++) {
            int64_t sum = 0;
            for (k = 0; k < nrows; k++)
                sum += A->vals[i * nrows + k] * B->vals[k * nrows + j];
            C->vals[i * nrows + j] = sum;
        }
    }
}

void jki_dgemm (dense *A, dense *B, dense *C) {
    int32_t i,j,k;
    int32_t nrows = A->nrows;
    for (j = 0; j < nrows; j++) {
        for (k = 0; k < nrows; k++) {
            int64_t r = B->vals[k * nrows + j];
            for (i = 0; i < nrows; i++)
                C->vals[i * nrows + j] += A->vals[i * nrows + k] * r;
        }
    }
}

void  kji_dgemm (dense *A, dense *B, dense *C) {
    int32_t i,j,k;
    int32_t nrows = A->nrows;
    for (k = 0; k < nrows; k++) {
        for (j = 0; j < nrows; j++) {
            int64_t r = B->vals[k * nrows + j];
            for (i = 0; i < nrows; i++)
                C->vals[i * nrows + j] += A->vals[i * nrows + k] * r;
        }
    }
}

int main (int argc, char**argv) {
    assert(argc == 5);
    char *A_name = argv[1];
    char *B_name = argv[2];
    char *C_name = argv[3];
    int tag = atoi(argv[4]);

    dense A, B, C;
    read_matrix(A_name, &A);
    read_matrix(B_name, &B);
    assert (A.nrows == B.nrows);
    assert (A.type == B.type);
    C.nrows = A.nrows;
    C.vals = (int32_t *)calloc(C.nrows * C.nrows, sizeof(int32_t));
    C.type = A.type;
    double t1 = 0.0, t2 = 0.0;
//    print(&A);
//    print(&B);
    switch (tag) {
        case IJK:
            t1 = timer();
            ijk_dgemm(&A, &B, &C);
            t2 = timer();
            break;
        case IKJ:
            t1 = timer();
            ikj_dgemm(&A, &B, &C);
            t2 = timer();
            break;
        case KIJ:
            t1 = timer();
            kij_dgemm(&A, &B, &C);
            t2 = timer();
            break;
        case JIK:
            t1 = timer();
            jik_dgemm(&A, &B, &C);
            t2 = timer();
            break;
        case JKI:
            t1 = timer();
            jki_dgemm(&A, &B, &C);
            t2 = timer();
            break;
        case KJI:
            t1 = timer();
            kji_dgemm(&A, &B, &C);
            t2 = timer();
            break;
        default:
            break;
    }
    printf("Elapsed time: %.5f\n", t2-t1);
//    print(&C);
    write_matrix(C_name, &C);
    free(A.vals);
    free(B.vals);
    free(C.vals);
    return 0;
}
