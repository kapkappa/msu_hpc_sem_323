#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_THREADS_NUMBER omp_get_max_threads()

double time_start, time_end;
int nthreads = 1;

void fill_array(int64_t* array, int array_size) {
    int i;
    srand(123);
    for (i = 0; i < array_size; i++) {
        array[i] = rand();
    }
}

parallel_sort(int64_t*array, int array_size) {
    int i;

     omp_set_num_threads(nthreads);
     printf("nthreads: %d\n", nthreads);

#pragma omp parallel for private(i) shared(array) schedule(static)
        for (i = 0; i < array_size; i++)
            printf("%d ", array[i]);

    printf("\n");

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


    int64_t* array = (int64_t*)malloc(sizeof(int64_t) * array_size);
    fill_array(array, array_size);

    time_start = omp_get_wtime();
    parallel_sort(array, array_size);
    time_end = omp_get_wtime();

    printf("sort time: %f\n", time_end - time_start);
    free(array);
    return 0;
}
