
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

CC=gcc
CFLAGS=-g -O0 -Wall -Wextra -pedantic -Werror -std=gnu11 -Wno-unused-function
LIBS=

all: main

main: $(OBJECTS)
	$(CC) $(OBJECTS) -o a.out $(CFLAGS) $(LIBS)

$(OBJECTS): ./%.o : ./%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm *.o; rm a.out

run:
	./a.out --cache-tests

run_all:
	./a.out --cache-tests --dbll-tests

gdb:
	gdb --args ./a.out --cache-tests

valg:
	valgrind --track-origins=yes --leak-check=full ./a.out --cache-tests
