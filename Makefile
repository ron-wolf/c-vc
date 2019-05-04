CC=gcc

all: build

build:
	$(CC) WTF.c       -o WTF
	$(CC) WTFserver.c -o WTFserver

test:
	$(CC) WTFtest.c   -o WTFtest

clean:
	rm -f WTF WTFserver WTFtest *.o
