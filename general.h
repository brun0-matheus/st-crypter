#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

/* OS Detection (Just detect Unix-like systems) */
#if (defined __unix__) || (defined unix) || (defined __linux__) || (defined linux)
 #define UNIX
#endif

/* Crypto configs */

#define IV {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}
#define KEY_SIZE 16
// Obs: If you change the KEY_SIZE, you will need to change the algoritm in aes.h
// Read the aes.h README for details, it's on the creator's github.

/* Error */

#ifdef MAIN_FILE
 char curNames[10][101] = {{0}};
 int cnPos = -1;
#else
 extern char curNames[10][101];
 extern int cnPos;
#endif

#define PRINT_ERROR 1

#if defined PRINT_ERROR && PRINT_ERROR != 0
 #define setCurName(n) strncpy(curNames[++cnPos], n, 101)
 #define rmCurName() memset(curNames[cnPos--], 0, 101)
 #define error() {fprintf(stderr, "Error code: %d \n%s\n", errno, strerror(errno));}
 #define fatal(er, ...) { if(PRINT_ERROR) { fprintf(stderr, "[%s] ", curNames[cnPos]); fprintf(stderr, __VA_ARGS__); \
                          if(er) error(); exit(1); }}
#endif
