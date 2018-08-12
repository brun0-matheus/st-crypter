all:
	gcc main.c -Wall -std=c99 -L./lib -lstatic -o st-crypter

library:
	make -C ./lib

clean:
	make -C ./lib clean
	rm -f st-crypter.exe
	rm -f st-crypter
