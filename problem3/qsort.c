#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

double time_start, time_end;
int nthreads = 1;

double timer() {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

void fill_array(int* array, int array_size) {
    int i;
    for (i = 0; i < array_size; i++) {
        array[i] = rand();
    }
}

int compare_ints(const void* a, const void* b)
{
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;

    // return (arg1 > arg2) - (arg1 < arg2); // possible shortcut
    // return arg1 - arg2; // erroneous shortcut (fails if INT_MIN is present)
}

int is_sorted(int* array, int array_size) {
    int i;
    for (i = 1; i < array_size; i++)
        if (array[i-1] > array[i]) return 0;
    return 1;
}

int main(int argc, char**argv) {
    srand(123);

    int array_size;
    array_size  = (argc > 1) ? atoi(argv[1]) : 10000;
    assert(array_size > 0);

    int* array = (int*)malloc(sizeof(int) * array_size);
    if(!array) return 1;

    fill_array(array, array_size);

    time_start = timer();

       qsort(array, array_size, sizeof(int), compare_ints);

    time_end = timer();

    assert(is_sorted(array, array_size) == 1);

    printf("sort time: %f\n", time_end - time_start);
    free(array);
    return 0;
}
