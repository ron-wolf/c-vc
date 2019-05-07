all: build

debug:
	@ CFLAGS='-Wall'
	gcc $(CFLAGS) WTF.c -o WTF
	gcc $(CFLAGS) WTFserver.c -o WTFserver

build: WTF.c WTFserver.c hash.o
	gcc WTF.c       -o WTF			hash.o
	gcc WTFserver.c -o WTFserver	hash.o

hash.o: hash.c
	gcc -c hash.c

test:
	$(CC) WTFtest.c   -o WTFtest

clean:
	rm -f WTF WTFserver WTFtest *.o
