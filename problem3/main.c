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

void fill(int *array, int array_size) {
  int i;
  for (i = 0; i < array_size; ++i)
    array[i] = array_size - i;
}

void print_array(int *array, int array_size) {
  int i;
  for (i = 0; i < array_size; i++)
    printf("%d ", array[i]);
  printf("\n");
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

void merge(int *array, int left_array_size, int merged_array_size) {
  int i = 0, j = left_array_size, ti = 0;
  int *tmp = (int *)malloc(sizeof(int) * merged_array_size);
  assert(tmp != NULL);
  while (i < left_array_size && j < merged_array_size) {
    if (array[i] < array[j]) {
      tmp[ti++] = array[i++];
    } else {
      tmp[ti++] = array[j++];
    }
  }
  for (i; i < left_array_size; i++) {
    tmp[ti++] = array[i];
  }
  for (j; j < merged_array_size; j++) {
    tmp[ti++] = array[j];
  }
  memcpy(array, tmp, merged_array_size * sizeof(int));
  free(tmp);
}

void recursive_merge(int *array, int left_array_number, int right_array_number,
                     int base_array_size) {

  int chunk_size = base_array_size / nthreads;
  int extra_chunk = base_array_size % nthreads;
  int left_border = chunk_size * left_array_number;
  if (left_border != 0)
    left_border += extra_chunk;

  if ((left_array_number + 1) < right_array_number) {
    int middle_array_number = right_array_number / 2;
#pragma omp task shared(array)
    recursive_merge(array, left_array_number, middle_array_number,
                    base_array_size);
#pragma omp task shared(array)
    recursive_merge(array, middle_array_number + 1, right_array_number,
                    base_array_size);
#pragma omp taskwait
    int left_array_size =
        chunk_size * (middle_array_number - left_array_number + 1);
    if (left_array_number == 0)
      left_array_size += extra_chunk;
    int merged_array_size =
        left_array_size +
        chunk_size * (right_array_number - middle_array_number);
    merge(array + left_border, left_array_size, merged_array_size);
  } else {
    if (right_array_number == left_array_number)
      return;

    int left_array_size = chunk_size;
    if (left_array_number == 0)
      left_array_size += extra_chunk;

    int merged_array_size = left_array_size + chunk_size;
    merge(array + left_border, left_array_size, merged_array_size);
  }
}

void merge_sort(int *array, int array_size) {
  if (array_size < 2)
    return;
  int i;
  int chunk_size = array_size / nthreads;
  int extra_chunk = array_size % nthreads;

  for (i = 0; i < nthreads; i++) {
    if (i == 0) {
      chunk_size += extra_chunk;
      extra_chunk = 0;
    } else {
      chunk_size = array_size / nthreads;
      extra_chunk = array_size % nthreads;
    }

#pragma omp task shared(array)
    qsort(array + i * chunk_size + extra_chunk, chunk_size, sizeof(int),
          compare_ints);
  }

#pragma omp taskwait
  recursive_merge(array, 0, nthreads - 1, array_size);
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
  assert(array != NULL);

  //  fill_array(array, array_size);
  fill(array, array_size);

  double time_start, time_end;

  time_start = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp single
    merge_sort(array, array_size);
  }
  time_end = omp_get_wtime();

  assert(is_sorted(array, array_size) == 1);

  printf("parallel sort time: %f\n", time_end - time_start);
  free(array);
  return 0;
}
