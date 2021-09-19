#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

int n, num_threads;
double h, pi;

pthread_mutex_t mutex;

double timer() {
    struct timeval tp;
    struct timezone tzp;
    gettimeofday(&tp, &tzp);
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
}

void *pi_calc(void *args) {
    int i, myid = *(int*)args;
    double sum = 0, x;
    for (i = myid; i < n; i += num_threads) {
        x = h * i;
        sum += 4.0/(1.0+x*x);
    }
    sum *= h;
    pthread_mutex_lock(&mutex);
    pi += sum;
    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}

int main (int argc, char**argv) {
    assert(argc == 3);

    double t1 = timer();

    n = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    h = 1.0/n, pi = 0;
    int i;
    pthread_t tid[num_threads];
    pthread_mutex_init(&mutex, NULL);

    int id[num_threads];

    for (i = 0; i < num_threads; i++) {
        id[i] = i;
        int ret_val = pthread_create(tid+i, NULL, pi_calc, (void*)&id[i] );
        assert(ret_val == 0);
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(tid[i], NULL);

    double t2 = timer();

    printf("pi=%.16f\n", pi);
    printf("Elapsed time: %.5f\n", t2-t1);
    return 0;
}
