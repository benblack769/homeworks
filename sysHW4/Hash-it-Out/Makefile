# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

all: m.o c.o l.o t.o
	$(CC) m.o c.o l.o t.o -o test.out

# the build target executable:
c.o:
	$(CC) -c $(CFLAGS) cache.c -o c.o

l.o:
	$(CC) -c $(CFLAGS) lru.c -o l.o

t.o:
	$(CC) -c $(CFLAGS) test.c -o t.o

m.o:
	$(CC) -c $(CFLAGS) main.c -o m.o


clean:
	$(RM) m.o c.o l.o t.o