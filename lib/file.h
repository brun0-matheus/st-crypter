#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../general.h"

typedef int UNIV_FILE;

typedef unsigned int SIZ;
#define MAX_SIZ 500 * 1048576  // 500MB

#ifdef UNIX 
 #define O_BINARY 0
#endif

#define M_READ 0  // Open file for reading
#define M_WRITE 1  // Open file for writing and truncate it
#define M_WRITE_NEW 2  // Open file for writing, create it if doesn't exist, otherwise truncate it

UNIV_FILE file_open(const char *path, const int mode);
SIZ file_getSize(UNIV_FILE file);
bool file_exists(const char *path);
void file_read(void *ptr, SIZ size, UNIV_FILE file);
void file_write(const void *ptr, SIZ size, UNIV_FILE file);
void file_close(UNIV_FILE file);
