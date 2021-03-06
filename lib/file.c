#include "file.h"

UNIV_FILE file_open(const char *path, const int mode) {
    int oflag = O_BINARY;

    switch(mode) {
        case M_READ:
            oflag |= O_RDONLY;
            break;
        case M_WRITE:
            oflag |= O_WRONLY;
            break;
        case M_WRITE_NEW:
            oflag |= O_WRONLY | O_CREAT;
            break;
        default:
            fatal(0, "FATAL: Unknown mode %d.\n", mode);
    }

    UNIV_FILE file = open(path, oflag);
    if(file == -1)
        fatal(1, "FATAL: Could not open the file.\n");
    
    return file;
}

SIZ file_getSize(UNIV_FILE file) {
    struct stat buf;

    fstat(file, &buf);
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
    ssize_t ret = read(file, ptr, size);
    if(ret == -1)
        fatal(1, "FATAL: Could not read the first %u bytes from the file.\n", size);
    
    lseek(file, 0, SEEK_SET);
}

void file_write(const void *ptr, SIZ size, UNIV_FILE file) {
    ssize_t ret = write(file, ptr, size);
    if(ret == -1)
        fatal(1, "FATAL: Could not write %u bytes into the file.\n", size);
    
    lseek(file, 0, SEEK_SET);
}

void file_close(UNIV_FILE file) {
    close(file);
}
