all:
	gcc -c -Wall -std=c99 file.c
	gcc -c -Wall -std=c99 aes.c
	ar -cru libstatic.a aes.o file.o

clean:
	rm -f file.o
	rm -f aes.o
	rm -f libstatic.a
