#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int vec_sum;

pthread_mutex_t mutex;

struct info {
    int id;
    int *v;
    int size;
    int nthreads;
};

void *vec_calc(void *args) {
    struct info* inf = (struct info*)(args);
    int myid = inf->id;
    int size = inf->size;
    int sum = 0;
    int i;
    int nthreads = inf->nthreads;
    int*vec = inf->v;
    for (i = myid; i < size; i += nthreads) {
        sum += vec[i];
    }
    pthread_mutex_lock(&mutex);
    vec_sum += sum;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int vector_sum(int *v, int size, int nthreads) {
    pthread_t tid[nthreads];
    pthread_mutex_init(&mutex, NULL);

    int i;

    struct info infos[nthreads];

    for (i = 0; i < nthreads; i++) {
        infos[i].id = i;
        infos[i].v = v;
        infos[i].size = size;
        infos[i].nthreads = nthreads;
        int ret_val = pthread_create(tid+i, NULL, vec_calc, infos+i);
        assert(ret_val == 0);
    }

    for (int i = 0; i < nthreads; i++)
        pthread_join(tid[i], NULL);

    return vec_sum;
}

int main (int argc, char**argv) {
    assert(argc == 3);

    int vec_size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    vec_sum = 0;

    int *v = (int*)malloc(sizeof(int) * vec_size);
    int i;
    printf("Введите массив\n");
    for (i = 0; i < vec_size; i++) {
        scanf("%d", v+i);
    }

    vector_sum(v, vec_size, num_threads);

    printf("vec_sum:%d\n", vec_sum);
    return 0;
}

/*
restrict указывает, что на участок памяти, на который указывает указатель (с ключевым словом restrict) - не ссылаются другие указатели
Фактическое выполнение этого обеспечивает программмист, и если это нарушается - будет неопределенное поведение
*/
