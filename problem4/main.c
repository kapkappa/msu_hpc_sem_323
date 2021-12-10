#include "matrix.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <mpi.h>

int main (int argc, char** argv) {

    MPI_Init(&argc, &argv);

    if (argc != 2) {
        printf("Please, enter matrices!\n");
        MPI_Finalize();
        return 0;
    }

    int world_size; //number of procs
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int rank;       //rank of cur proc
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//READ SRC VECTOR
    uint32_t vrows, vcols;
    double *src = read_matrix(&vrows, &vcols, VECTOR, argv[1]);
    if (src == 0) {
        MPI_Finalize();
        return 1;
    }
//READ MATRIX
    double *local_val = NULL, *vals = NULL;

    uint32_t nrows = 0, ncols = 0;
    if (rank == 0) {
        vals = read_matrix(&nrows, &ncols, MATRIX, argv[1]);
    }

    MPI_Bcast(&nrows, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(&ncols, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    if (ncols == 0 && nrows == 0) {
        MPI_Finalize();
        return 2;
    }

    //I need chunks!
    int block_rows = nrows / world_size;
    int extra_rows = nrows % world_size;
    int rows = block_rows;
    if (rank == 0) {
        rows += extra_rows;    //root proc handle extra rows
        local_val = vals;
    } else {
        local_val = (double*)malloc(sizeof(double) * rows * ncols);
    }

    //Send Matrices to Procs!
    int sendcounts[world_size];
    int recvcounts[world_size];
    int disps[world_size];
    int l;
    for (l = 0; l < world_size; l++) {
        sendcounts[l] = block_rows * ncols;
        disps[l] = block_rows * ncols * l + extra_rows * ncols;
    }
    sendcounts[0] += extra_rows * ncols;
    MPI_Scatterv(vals, sendcounts, disps, MPI_DOUBLE, local_val, rows * ncols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

//DEST Vector
    double* dest = (double*)calloc(nrows, sizeof(double));
    double* work_dest = (double*)malloc(rows * sizeof(double));

    int i, j, k, niters = 3;
    for (k = 0; k < niters; k++) {
        MPI_Barrier(MPI_COMM_WORLD);
        printf("Iter: %d\n", k);
        double t1 = MPI_Wtime();

        for (i = 0; i < rows; i++) work_dest[i] = 0;

        for (i = 0; i < rows; i++) {
            for (j = 0; j < ncols; j++) {
                work_dest[i] += local_val[i*ncols + j] * src[j];
            }
        }

        for (i = 0; i < world_size; i++) {
            recvcounts[i] = block_rows;
            disps[i] = extra_rows + block_rows * i;
        }
        disps[0] -= extra_rows;
        recvcounts[0] += extra_rows;

        MPI_Allgatherv(work_dest, rows, MPI_DOUBLE, dest, recvcounts, disps, MPI_DOUBLE, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        double t2 = MPI_Wtime();

        printf("From rank [%d]: time: %f\n", rank, t2-t1);
    }

#ifdef DEBUG
    for (i = 0; i < vrows; i++)
        printf("%f ", dest[i]);
    printf("\n");
#endif

    if (rank == 0) free(vals);
    else free(local_val);
    free(src);
    free(dest);
    free(work_dest);
    MPI_Finalize();
    return 0;
}
