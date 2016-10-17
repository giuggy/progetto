CC = gcc
CFLAGS = -O2
LFLAGS = -l OpenCL

all: vecadd

vecadd: vecadd.c
	$(CC) $(CFLAGS) vecadd.c -o vecadd $(LFLAGS)

.phony: clean

clean:
	rm -f vecadd

