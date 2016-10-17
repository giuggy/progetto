CC = gcc
CFLAGS = -O2
LFLAGS = -l OpenCL
CLUT = ../clut/
OBJ = $(CLUT)clut.o

matmul: matmul.c
	$(CC) $(CFLAGS) -I$(CLUT) matmul.c $(OBJ) -o matmul $(LFLAGS)

.phony: clean

clean:
	rm -f matmul 
