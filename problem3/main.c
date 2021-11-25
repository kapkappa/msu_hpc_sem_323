#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double time_start, time_end;
int nthreads = 1;

void fill_array(int64_t* array, unsigned int array_size) {
    int i;
    srand(123);
    
    for (i = 0; i < array_size; i++) {
        array[i] = rand();
    }
}

parallel_sort(int64_t*array, unsigned int array_size) {
    int i;

     omp_set_num_threads(nthreads);
     printf("nthreads: %d\n", nthreads);


#pragma omp parallel for private(i) shared(array) schedule(static)
        for (i = 0; i < array_size; i++)
            printf("%d ", array[i]);

    printf("\n");

}

int main(int argc, char**argv) {
    unsigned int array_size = 0;
    do {
        printf("Enter valid array size\n");
        scanf("%d", &array_size);
    } while (array_size == 0);
    int64_t* array = (int64_t*)malloc(sizeof(int64_t) * array_size);
    fill_array(array, array_size);

    time_start = omp_get_wtime();
    parallel_sort(array, array_size);
    time_end = omp_get_wtime();

    printf("sort time: %f\n", time_end - time_start);
    free(array);
    return 0;
}
