CC = gcc
CFLAGS = -O3 -w -fopenmp


all: clean omp qsort

omp:
	$(CC) $(CFLAGS) main.c -o prog_omp

qsort:
	$(CC) $(CFLAGS) qsort.c -o prog_qs

clean:
	rm -f prog_omp prog_qs
