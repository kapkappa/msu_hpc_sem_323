#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <assert.h>

#define MAX_THREADS_NUMBER omp_get_max_threads()

double time_start, time_end;
int nthreads = 1;

void fill_array(int* array, int array_size) {
    int i;
    srand(123);
    for (i = 0; i < array_size; i++) {
        array[i] = rand();
    }
}

void merge(int* array, int array_size, int* tmp) {
    int i = 0, j = array_size / 2, ti = 0;

   while (i < array_size / 2 && j < array_size) {
      if (array[i] < array[j]) {
         tmp[ti] = array[i];
         ti++;
         i++;
      } else {
         tmp[ti] = array[j];
         ti++;
         j++;
      }
   }
   while (i<array_size/2) { /* finish up lower half */
      tmp[ti] = array[i];
      ti++;
      i++;
   }
   while (j<array_size) { /* finish up upper half */
      tmp[ti] = array[j];
      ti++;
      j++;
   }
   memcpy(array, tmp, array_size*sizeof(int));
}

void merge_sort(int* array, int array_size, int* tmp) {
    if (array_size < 2) return;

#pragma omp task shared(array) if(array_size > 100)
    merge_sort(array, array_size/2, tmp);

#pragma omp task shared(array) if(array_size > 100)
    merge_sort(array+(array_size/2), array_size - (array_size/2), tmp + (array_size/2));

#pragma omp taskwait
    merge(array, array_size, tmp);
}

int is_sorted(int* array, int array_size) {
    int i;
    for (i = 1; i < array_size; i++)
        if (array[i] < array[i-1]) return 0;
    return 1;
}

int main(int argc, char**argv) {
    int array_size = 0;
    nthreads = omp_get_max_threads();

    do {
        printf("Enter valid array size\n");
        scanf("%d", &array_size);
    } while (array_size <= 0);

    do {
        printf("Enter number of threads (max threads available: %d)\n", nthreads);
        scanf("%d", &nthreads);
    } while ((nthreads <=0) || (nthreads > MAX_THREADS_NUMBER ));

    omp_set_dynamic(0);
    omp_set_num_threads(nthreads);
    printf("nthreads: %d\n", nthreads);

    int* array = (int*)malloc(sizeof(int) * array_size);
    int* tmp = (int*)malloc(sizeof(int) * array_size);

    if (!array || !tmp) {
        if(array) free(array);
        if(tmp) free(tmp);
        return 1;
    }

    fill_array(array, array_size);

    time_start = omp_get_wtime();
#pragma omp parallel
{
    #pragma omp single
       merge_sort(array, array_size, tmp);
}
    time_end = omp_get_wtime();

    assert(is_sorted(array, array_size) == 1);

    printf("sort time: %f\n", time_end - time_start);
    free(array);
    free(tmp);
    return 0;
}
