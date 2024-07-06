CC = gcc
CFLAGS = -Iinclude

all: test_libcsv

test_libcsv: libcsv.c test_libcsv.c
	$(CC) $(CFLAGS) -o test_libcsv test_libcsv.c libcsv.c

clean:
	rm -f test_libcsv
