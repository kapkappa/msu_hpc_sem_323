CC = gcc
CFLAGS = -O3 -w -fopenmp


all: clean omp qsort format

omp:
	$(CC) $(CFLAGS) main.c -o prog_omp

qsort:
	$(CC) $(CFLAGS) qsort.c -o prog_qs

format:
	clang-format -i -style=LLVM *.c

clean:
	rm -f prog_omp prog_qs
