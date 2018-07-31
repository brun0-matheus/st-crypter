#include <stdio.h>
#include <stdlib.h>

/* Crypto configs */

#define IV {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}
#define KEY_SIZE 16
// Obs: If you change the KEY_SIZE, you will need to change the algoritm in aes.h
// Read the aes.h README for details, it's on the creator's github.

/* Error */

#define PRINT_ERROR 1

#if defined PRINT_ERROR && PRINT_ERROR != 0
 #define error() {printf("Error code: %d \n%s\n", errno, strerror(errno));}
 #define fatal(er, ...) { if(PRINT_ERROR) { fprintf(stderr, __VA_ARGS__);  if(er) error(); exit(1); }}
#endif
