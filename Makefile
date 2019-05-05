CC=gcc

all: build

debug:
	@ CFLAGS='-Wall'
	$(CC) $(CFLAGS) WTF.c -o WTF
	$(CC) $(CFLAGS) WTFserver.c -o WTFserver

build:
	$(CC) WTF.c       -o WTF
	$(CC) WTFserver.c -o WTFserver

test:
	$(CC) WTFtest.c   -o WTFtest

clean:
	rm -f WTF WTFserver WTFtest *.o
