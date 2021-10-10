#include "matrix.h"
#include <sys/time.h>

double timer() {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec+(double)tp.tv_usec * 1.e-6);
}

int main (int argc, char** argv) {
    if (argc != 2) {
        printf("Please, enter size of matrix!\n");
        return 0;
    }
//READ MATRIX
    uint32_t nrows, ncols;
    double *vals = read_matrix(&nrows, &ncols, MATRIX, argv[1]);
    if (vals == 0) {
        int status = 0;
        exit(status);
    }
//READ SRC VECTOR
    uint32_t vrows, vcols;
    double *src = read_matrix(&vrows, &vcols, VECTOR, argv[1]);
    if (src == 0) {
        int status = 0;
        exit(status);
    }
//DEST Vector
    double* dest = (double*)malloc(nrows * sizeof(double));

    int i, j, k;

    int niters = 3;
    for (k = 0; k < niters; k++) {
        for (i = 0; i < nrows; i++)
            dest[i] = 0;

        double t1 = timer();
        for (i = 0; i < nrows; i++) {
            for (j = 0; j < ncols; j++) {
                dest[i] += vals[i*ncols + j] * src[j];
            }
        }
        double t2 = timer();

        printf("Time: %f\n", t2-t1);
    }
#ifdef DEBUG
    for (i = 0; i < vrows; i++)
        printf("%f ", dest[i]);
    printf("\n");
#endif
    free(vals);
    free(src);
    free(dest);
    return 0;
}
