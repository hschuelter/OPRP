# GNU Makefile

CC = gcc
CCFLAGS = -Wall -pg
LDFLAGS = -Wall -pg -pthread
TARGET = main

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@

all: $(TARGET)

# Dependencias

matrix.o: matrix.c matrix.h
main: matrix.o main.c

clean:
	rm -f *.o *~ $(TARGET)
