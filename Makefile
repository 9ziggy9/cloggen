EXE=./test
CC=gcc
CFLAGS=-Wall -Wextra -pedantic -Wconversion

test: clean all
	./test

all: main.c
	$(CC) $(CFLAGS) $< -o $(EXE)

clean:
	rm -rf $(EXE) *.o *.so *.s
