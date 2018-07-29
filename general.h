#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Detect OS */

#define NO_OS_ERROR_MSG *** Nao foi possivel identificar o Sistema Operacional ***
#ifdef __WIN32
 #define WINDOWS
 #define OS_NAME "Windows"
 #include <winerror.h>
#elif defined __linux__
 #define LINUX
 #define OS_NAME "Linux"  // Yes, I know that Linux is a Kernel, but I wouldn't modify this name just because of this
 #include <string.h>
 #include <errno.h>
#else
 #define OS_NAME "None"
 #error NO_OS_ERROR_MSG
#endif

/* Crypto configs */

#define IV {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}
#define KEY_SIZE 16
#define CRYPT_ALG CALG_AES_128
// Obs: If you change the KEY_SIZE and CRYPT_ALG, you will need to change the algoritm in aes.h
// Read the aes.h README for details, it's on the creator's github.

/* Error */

#define PRINT_ERROR 1

// error() macro
#if defined PRINT_ERROR && PRINT_ERROR != 0
 #ifdef WINDOWS
  #define error() {char error_buff[1024]; DWORD error_code = GetLastError(); \
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |  FORMAT_MESSAGE_IGNORE_INSERTS, \
        NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error_buff, 1024, NULL); \
        printf("Error code: %d \n%s\n", error_code, error_buff); }
 #elif defined LINUX
  #define error() {printf("Error code: %d \n%s\n", errno, strerror(errno));}
 #endif
#else
 #define error() {}
#endif

// fatal() macro
#if defined PRINT_ERROR && PRINT_ERROR != 0
 #define fatal(er, ...) { if(PRINT_ERROR) { fprintf(stderr, __VA_ARGS__);  if(er) error(); exit(1); }}
#else
 #define fatal(er, ...) {};
#endif