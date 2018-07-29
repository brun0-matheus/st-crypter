#include "../general.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef WINDOWS
 #include <Windows.h>
 typedef HANDLE* UNIV_FILE;
#elif defined LINUX
 #include <fcntl.h>
 #include <unistd.h>
 #include <sys/stat.h>

 typedef int UNIV_FILE;
#else
 #error NO_OS_ERROR_MSG
#endif

typedef unsigned int SIZ;
#define MAX_SIZ 500 * 1048576  // 500MB

#define M_READ 0
#define M_WRITE 1
#define M_WRITE_NEW 2

UNIV_FILE file_open(const char *path, const int mode);
SIZ file_getSize(UNIV_FILE file);
bool file_exists(const char *path);
void file_read(void *ptr, SIZ size, UNIV_FILE file);
void file_write(const void *ptr, SIZ size, UNIV_FILE file);
void file_close(UNIV_FILE file);
