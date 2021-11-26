#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_THREADS_NUMBER omp_get_max_threads()
int nthreads = 1;

void fill_array(int *array, int array_size) {
  int i;
  for (i = 0; i < array_size; i++) {
    array[i] = rand();
  }
}

int compare_ints(const void *a, const void *b) {
  int arg1 = *(const int *)a;
  int arg2 = *(const int *)b;
  if (arg1 < arg2)
    return -1;
  if (arg1 > arg2)
    return 1;
  return 0;
}

void merge(int *array, int middle, int array_size, int *tmp) {
  int i = 0, j = middle, ti = 0;

  while (i < middle && j < array_size) {
    if (array[i] < array[j]) {
      tmp[ti++] = array[i++];
    } else {
      tmp[ti++] = array[j++];
    }
  }
  for (i; i < middle; i++) {
    tmp[ti++] = array[i];
  }
  for (j; j < array_size; j++) {
    tmp[ti++] = array[j];
  }
  memcpy(array, tmp, array_size * sizeof(int));
}

void merge_sort(int *array, int array_size, int *tmp) {
  if (array_size < 2)
    return;
  int i = 0;
  int chunk_size = array_size / nthreads;
  int extra_chunk = array_size % nthreads;

  for (i = 0; i < nthreads; i++) {
    if (i == 0) {
      chunk_size += extra_chunk;
      extra_chunk = 0;
    }
#pragma omp task shared(array)
    qsort(array + i * chunk_size + extra_chunk, chunk_size, sizeof(int),
          compare_ints);
  }

#pragma omp taskwait
  for (i = 1; i < nthreads; i++) {
    int current_border = chunk_size * i + extra_chunk;
    int current_size = current_border + chunk_size;
    merge(array, current_border, current_size, tmp);
  }
}

int is_sorted(int *array, int array_size) {
  int i;
  for (i = 1; i < array_size; i++)
    if (array[i] < array[i - 1])
      return 0;
  return 1;
}

int main(int argc, char **argv) {
  srand(123);

  int array_size;

  array_size = (argc > 1) ? atoi(argv[1]) : 1000000;
  assert(array_size > 0);
  nthreads = (argc > 2) ? atoi(argv[2]) : 1;
  assert(nthreads > 0 && nthreads <= MAX_THREADS_NUMBER);

  omp_set_dynamic(0);
  omp_set_num_threads(nthreads);
  printf("nthreads: %d\n", nthreads);

  int *array = (int *)malloc(sizeof(int) * array_size);
  int *tmp = (int *)malloc(sizeof(int) * array_size);

  if (!array || !tmp) {
    if (array)
      free(array);
    if (tmp)
      free(tmp);
    return 1;
  }

  fill_array(array, array_size);

  double time_start, time_end;

  time_start = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp single
    merge_sort(array, array_size, tmp);
  }
  time_end = omp_get_wtime();

  assert(is_sorted(array, array_size) == 1);

  printf("parallel sort time: %f\n", time_end - time_start);
  free(array);
  free(tmp);
  return 0;
}
