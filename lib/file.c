#include "file.h"

UNIV_FILE file_open(const char *path, const int mode) {
    char m[5] = {0};

    switch(mode) {
        case M_READ:
            strcpy(m, "rb");
            break;
        case M_WRITE:
            strcpy(m, "wb");
            break;
        case M_WRITE_NEW:
            strcpy(m, "w+b");
            break;
        default:
            fatal(0, "FATAL: Unknown mode %d.\n", mode);
    }

    UNIV_FILE file = fopen(path, m);
    if(file == NULL)
        fatal(1, "FATAL: Could not open the file.\n", path);
    
    return file;
}

SIZ file_getSize(UNIV_FILE file) {
    struct stat buf;
    int fd = fileno(file);

    fstat(fd, &buf);
    SIZ size = buf.st_size;

    if(size > MAX_SIZ)
        fatal(0, "FATAL: The file's size exceeded the limit.\n");

    return size;
}

bool file_exists(const char *path) {
    bool ret = access(path, F_OK) != -1;
    errno = 0;

    return ret;
}

void file_read(void *ptr, SIZ size, UNIV_FILE file) {
    int fd = fileno(file);

    ssize_t ret = read(fd, ptr, size);
    if(ret == -1)
        fatal(1, "FATAL: Could not read the first %u bytes from the file.\n", size);
    
    rewind(file);
}

void file_write(const void *ptr, SIZ size, UNIV_FILE file) {
    int fd = fileno(file);

    ssize_t ret = write(fd, ptr, size);
    if(ret == -1)
        fatal(1, "FATAL: Could not write %u bytes into the file.\n", size);
    
    rewind(file);
}

void file_close(UNIV_FILE file) {
    fclose(file);
}
