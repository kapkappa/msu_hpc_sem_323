#CC = gcc
#CC = clang
#CC = icpc
CFLAGS = -g -Wall -Wextra -Wpedantic -Werror -pthread

all: clean prog test

prog:
	$(CC) $(CFLAGS) main.c -o prog

test:
	$(CC) $(CFLAGS) -fsanitize=address main.c -o test
	./test.sh

clean:
	rm -f prog test
