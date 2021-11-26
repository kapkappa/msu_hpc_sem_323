CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic -Werror -pthread

all: clean prog

prog:
	$(CC) $(CFLAGS) main.c -o prog

clean:
	rm -f prog
