CC     = gcc
CFLAGS = -O3 -Wall

all: md5crack md5crack_verbose

md5crack: md5.o enum.o main.o sort.o
	$(CC) $(CFLAGS) main.o md5.o enum.o sort.o -o md5crack

md5crack_verbose: md5.o enum_v.o main.o sort.o
	$(CC) $(CFLAGS) main.o md5.o enum_v.o sort.o -o md5crack_verbose
	
enum_v.o: enum.c enum.h sort.h
	$(CC) $(CFLAGS) -DVERBOSE -c enum.c -o enum_v.o

enum.o: enum.c enum.h sort.h
	$(CC) $(CFLAGS) -c enum.c

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) -c md5.c
	
main.o: main.c md5.h enum.h sort.h
	$(CC) $(CFLAGS) -c main.c
	
sort.o: sort.c sort.h
	$(CC) $(CFLAGS) -c sort.c		

.PHONY : test test-osx
test:
	@rm -f list.tmp	
	@echo -n 'pctips' | md5sum >> list.tmp
	@echo -n 'a5' | md5sum >> list.tmp
	@echo -n 'uno' | md5sum >> list.tmp
	@echo -n 'lupin71' | md5sum >> list.tmp
	@echo -n 'x' | md5sum >> list.tmp
	@echo -n 'test' | md5sum >> list.tmp
	@echo -n 'ida13' | md5sum >> list.tmp
	@./md5crack_verbose list.tmp

test-osx:
	@rm -f list.tmp	
	md5 -q -s pctips >> list.tmp
	md5 -q -s a5 >> list.tmp
	md5 -q -s uno >> list.tmp
	md5 -q -s lupin71 >> list.tmp
	md5 -q -s x >> list.tmp
	md5 -q -s test >> list.tmp
	@./md5crack_verbose list.tmp

.PHONY : clean	
clean:
	rm -f md5crack md5crack_verbose *.o list.tmp
