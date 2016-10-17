CC     = gcc
CFLAGS = -O3 -Wall
LFLAGS = -l OpenCL

convolution1: main.o clut.o pgm.o convolution.o
	$(CC) $(CFLAGS) main.o clut.o pgm.o convolution.o \
        -o convolution $(LFLAGS)

main.o: main.c clut.h convolution.h pgm.h
	$(CC) $(CFLAGS) main.c -c

convolution.o: convolution.c convolution.h clut.h
	$(CC) $(CFLAGS) convolution.c -c

clut.o: clut.c clut.h
	$(CC) $(CFLAGS) clut.c -c

pgm.o: pgm.c pgm.h
	$(CC) $(CFLAGS) pgm.c -c

.phony: clean

clean:
	rm -f convolution *.o results/*
