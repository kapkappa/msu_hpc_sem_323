#include <math.h>
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int32_t *read_matrix(char *filename, uint32_t *nrows, uint32_t *ncols) {
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
  int32_t *vals = NULL;
  if (type == 'l') {
    vals = (int32_t *)malloc(elems * sizeof(int32_t));
    if (!vals) {
      printf("malloc error\n");
      return NULL;
    }
    for (i = 0; i < elems; i++) {
      cnt += fread(&vals[i], sizeof(int32_t), 1, file);
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
                  int32_t *vals) {
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
      cnt += fwrite(&vals[i], sizeof(int32_t), 1, file);
    }
  } else {
    printf("Incorrect data type\n");
  }
  if (cnt != elems) {
    perror("incorrect number of elemets written\n");
  }
  fclose(file);
}

void print(uint32_t nrows, uint32_t ncols, int32_t *vals) {
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

  int world_size, cart_size, rank;
  int dim[3], period[3], coords[3];

  MPI_Comm cart;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  cart_size = (int)cbrtf(world_size);
  dim[0] = cart_size;
  dim[1] = cart_size;
  dim[2] = cart_size;
  period[0] = 0;
  period[1] = 0;
  period[2] = 0;

  assert(MPI_Cart_create(MPI_COMM_WORLD, 3, dim, period, 1, &cart) == MPI_SUCCESS);
  MPI_Comm_rank(cart, &rank);
  assert(MPI_Cart_coords(cart, rank, 3, coords) == MPI_SUCCESS);

  char *A_name = argv[1];
  char *B_name = argv[2];
  char *C_name = argv[3];

  // READ MATRIX
  uint32_t nrows = 0, ncols = 0;
  int32_t *A_local = NULL, *A_vals = NULL;
  int32_t *B_local = NULL, *B_vals = NULL;
  int32_t *C_local = NULL, *C_vals = NULL;
  if (rank == 0) {
    A_vals = read_matrix(A_name, &nrows, &ncols);
    A_local = A_vals;

    B_vals = read_matrix(B_name, &nrows, &ncols);
    B_local = B_vals;

  } else {
    A_local = (int32_t*)malloc(nrows * sizeof(int32_t));
    B_local = (int32_t*)malloc(nrows * sizeof(int32_t));
    C_local = (int32_t*)malloc(nrows * sizeof(int32_t));
  }

  MPI_Bcast(&nrows, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ncols, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

  if (ncols == 0 || nrows == 0 || nrows == ncols || nrows != cart_size) {
    MPI_Finalize();
    return 1;
  }

  // Send Matrices to Procs!
  int recvcounts[world_size];
  int disps[world_size];
  int l;
  for (l = 0; l < world_size; l++) {
    sendcounts[l] = block_rows * ncols;
    disps[l] = block_rows * ncols * l + extra_rows * ncols;
  }
  sendcounts[0] += extra_rows * ncols;
  disps[0] = 0;
  MPI_Scatter(A_vals, nrows, disps, MPI_INT32_T, A_local, nrows,
               MPI_INT32_T, 0, cart);



  //DGEMM
  int i, j, k, niters = 3;
  for (k = 0; k < niters; k++) {
    MPI_Barrier(MPI_COMM_WORLD);
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

    printf("On iteration: [%d], from rank [%d]: time: %f\n", k, rank, t2 - t1);
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
