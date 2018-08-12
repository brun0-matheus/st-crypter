# st-crypter
A Scantime Crypter in C that uses AES encryption (128bit key by default).
The AES encryption source code was built by @kokke. Click [here](https://github.com/kokke/tiny-AES-c) to go to the repository.

## How does it work?
A Scantime Crypter is a program that encrypts a binary, then generates a stub (in this case, an c source code file) and put the encrypted binary into it. When you compile and run the stub, it will decrypt the binary, drop it somewhere and execute. It is used to obfuscate a virus.

The AES encryption uses, by default, a 128bit (16 bytes) key. If you want to change it, just change the macro *KEY_SIZE* in `general.h` to fit the new key size (it must be 16, 24 or 32) and also change the AES algoritm in `lib/aes.h` (If you don't know how to do this, read the README of this AES implementation under https://github.com/kokke/tiny-AES-c/blob/master/README.md).

This spefic program use libraries that probably are not in the victim's computer, so it will have to be compiled using static libraries, and not shared ones. Be careful!

## Installation
Here we will use the gcc compiler, and the commands will be the same in both Windows and Linux systems.

First, compile the libraries. You can use `make library`. They need to be static and use the c99 standard:
```
st-crypter/lib> gcc -c -Wall -std=c99 file.c
st-crypter/lib> gcc -c -Wall -std=c99 aes.c
st-crypter/lib> ar -cru libstatic.a aes.o file.o
```

Now, compile the main program. You can use `make`:
```
st-crypter> gcc main.c -Wall -std=c99 -L./lib -lstatic -o st-crypter
```

And it's done! To remove the compiled binaries, you can use `make clean` or delete them manually.

## How to use
After it is compiled, you just run the program passing the filename of the binary. You can use the option --key \<file\> to set a key that is located in \<file\>. If you don't use this option, the key will be random. You also can pass the option --path \<path\> to set the output path of the binary that will be dropped by the stub. If you don't pass this option, the path will be the one defined in `main.c` by the macro *DEF_EXE_PATH_NAME*.

After the execution, a stub will be generated (by default, it will looks like "out_stub?.c"). Now, you just have to compile the stub. You can use the suggested commands that the program will prompt to you.
