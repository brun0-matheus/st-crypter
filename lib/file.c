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
            fatal(0, "FATAL: Modo %d desconhecido.\n", mode);
    }

    UNIV_FILE file = fopen(path, m);
    if(file == NULL)
        fatal(1, "FATAL: Nao foi possivel abrir o arquivo.\n");
    
    return file;
}

SIZ file_getSize(UNIV_FILE file) {
    fseek(file, 0, SEEK_END);
    SIZ size = ftell(file);
    rewind(file);

    if(size > MAX_SIZ)
        fatal(0, "FATAL: O tamanho do arquivo excedeu o limite.\n");

    return size;
}

bool file_exists(const char *path) {
    return access(path, F_OK) != -1;
}

void file_read(void *ptr, SIZ size, UNIV_FILE file) {
    int fd = fileno(file);

    ssize_t ret = read(fd, ptr, size);
    if(ret == -1)
        fatal(1, "FATAL: Nao foi possivel ler os primeiros %u bytes do arquivo.\n", size);
    
    rewind(file);
}

void file_write(const void *ptr, SIZ size, UNIV_FILE file) {
    int fd = fileno(file);

    ssize_t ret = write(fd, ptr, size);
    if(ret == -1)
        fatal(1, "FATAL: Nao foi possivel escrever %u bytes no arquivo.\n", size);
    
    rewind(file);
}

void file_close(UNIV_FILE file) {
    fclose(file);
}
