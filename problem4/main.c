#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int64_t *read_matrix(char *filename, uint32_t *nrows, uint32_t *ncols) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Can't open matrix file\n");
    return NULL;
  }

  int64_t cnt = 0;
  char type;
  cnt += fread(&type, sizeof(char), 1, file);
  cnt += fread(nrows, sizeof(uint32_t), 1, file);
  cnt += fread(ncols, sizeof(uint32_t), 1, file);
  if (cnt != 3) {
    printf("incorrect read of matrix parameters\n");
    return NULL;
  }
  int64_t i;
  int64_t elems = (*nrows) * (*ncols);
  cnt = 0;
  int64_t *vals = NULL;
  if (type == 'l') {
    vals = (int64_t *)malloc(elems * sizeof(int64_t));
    if (!vals) {
      printf("malloc error\n");
      return NULL;
    }
    for (i = 0; i < elems; i++) {
      cnt += fread(&vals[i], sizeof(int64_t), 1, file);
    }
  } else {
    printf("Incorrect data type\n");
  }
  if (cnt != elems) {
    printf("incorrect number of elements read\n");
    return NULL;
  }
  fclose(file);
  return vals;
}

void write_matrix(char *filename, uint32_t nrows, uint32_t ncols,
                  int64_t *vals) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    perror("cant open file\n");
  }
  int64_t cnt = 0;
  char type = 'l';
  cnt += fwrite(&type, sizeof(char), 1, file);
  cnt += fwrite(&nrows, sizeof(uint32_t), 1, file);
  cnt += fwrite(&ncols, sizeof(uint32_t), 1, file);
  if (cnt != 3) {
    perror("incorrect writing matrix parameters\n");
  }
  int64_t i, elems = nrows * ncols;
  cnt = 0;
  if (type == 'l') {
    for (i = 0; i < elems; i++) {
      cnt += fwrite(&vals[i], sizeof(int64_t), 1, file);
    }
  } else {
    printf("Incorrect data type\n");
  }
  if (cnt != elems) {
    perror("incorrect number of elemets written\n");
  }
  fclose(file);
}

void print(uint32_t nrows, uint32_t ncols, int64_t *vals) {
  uint32_t i, j;
  printf("Matrix size : %d %d\n", nrows, ncols);
  for (i = 0; i < nrows; i++) {
    for (j = 0; j < ncols; j++) {
      printf("%ld ", vals[i * nrows + j]);
    }
    printf("\n");
  }
}

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  if (argc != 4) {
    printf("Need 3 args!\n");
    printf("Enter source matrix, vector and destination vector\n");
    MPI_Finalize();
    return 0;
  }

  int world_size; // number of procs
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int rank; // rank of cur proc
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  char *A_name = argv[1];
  char *b_name = argv[2];
  char *y_name = argv[3];

  // READ SRC VECTOR
  uint32_t vrows, vcols;
  int64_t *source_vector = read_matrix(b_name, &vcols, &vrows);
  if (!source_vector) {
    MPI_Finalize();
    return 1;
  }
  // READ MATRIX
  int64_t *local_val = NULL, *vals = NULL;
  uint32_t nrows = 0, ncols = 0;
  if (rank == 0) {
    vals = read_matrix(A_name, &nrows, &ncols);
  }

  MPI_Bcast(&nrows, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ncols, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

  if (ncols == 0 && nrows == 0) {
    MPI_Finalize();
    return 2;
  }

  // I need chunks!
  int block_rows = nrows / world_size;
  int extra_rows = nrows % world_size;
  int rows = block_rows;
  if (rank == 0) {
    rows += extra_rows; // root proc handle extra rows
    local_val = vals;
  } else {
    local_val = (int64_t *)malloc(sizeof(int64_t) * rows * ncols);
  }

  // Send Matrices to Procs!
  int sendcounts[world_size];
  int recvcounts[world_size];
  int disps[world_size];
  int l;
  for (l = 0; l < world_size; l++) {
    sendcounts[l] = block_rows * ncols;
    disps[l] = block_rows * ncols * l + extra_rows * ncols;
  }
  sendcounts[0] += extra_rows * ncols;
  disps[0] = 0;
  MPI_Scatterv(vals, sendcounts, disps, MPI_INT64_T, local_val, rows * ncols,
               MPI_INT64_T, 0, MPI_COMM_WORLD);

  // DEST Vector
  int64_t *dest = (int64_t *)calloc(nrows, sizeof(int64_t));
  int64_t *work_dest = (int64_t *)malloc(rows * sizeof(int64_t));

  int i, j, k, niters = 3;
  for (k = 0; k < niters; k++) {
    MPI_Barrier(MPI_COMM_WORLD);
    printf("Iter: %d\n", k);
    double t1 = MPI_Wtime();

    for (i = 0; i < rows; i++)
      work_dest[i] = 0;

    for (i = 0; i < rows; i++) {
      for (j = 0; j < ncols; j++) {
        work_dest[i] += local_val[i * ncols + j] * source_vector[j];
      }
    }

    for (i = 0; i < world_size; i++) {
      recvcounts[i] = block_rows;
      disps[i] = extra_rows + block_rows * i;
    }
    disps[0] -= extra_rows;
    recvcounts[0] += extra_rows;

    MPI_Allgatherv(work_dest, rows, MPI_INT64_T, dest, recvcounts, disps,
                   MPI_INT64_T, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = MPI_Wtime();

    printf("From rank [%d]: time: %f\n", rank, t2 - t1);
  }

  if (rank == 0)
    write_matrix(y_name, vrows, vcols, work_dest);

  MPI_Barrier(MPI_COMM_WORLD);

#ifdef DEBUG
  if (rank == 0)
    print(vrows, vcols, work_dest);
#endif

  if (rank == 0)
    free(vals);
  else
    free(local_val);
  free(source_vector);
  free(dest);
  free(work_dest);
  MPI_Finalize();
  return 0;
}
