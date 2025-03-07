EXE    = ./test
CC     = gcc
CFLAGS = -Wall -Wextra -pedantic -Wconversion
SRCS   = cloggen.c
OBJS   = $(SRCS:.c=.o)
LIBS   = -lm

test: clean all
	./test

all: $(OBJS) main.c
	$(CC) $(CFLAGS) main.c -o $(EXE) $(OBJS) $(LIBS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $<

pre: clean main.c
	$(CC) -E main.c $(CFLAGS) > main.i

clean:
	rm -rf $(EXE) *.o *.so *.s *.i
