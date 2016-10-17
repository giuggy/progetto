CC = gcc
CFLAGS = -O2
LFLAGS = -l OpenCL

devinfo: devinfo.c
	$(CC) $(CFLAGS) devinfo.c -o devinfo $(LFLAGS)

.phony: clean

clean:
	rm -f devinfo 
