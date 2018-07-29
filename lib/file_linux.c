#include "file.h"

#ifndef LINUX
 #error *** Voce esta tentando compilar uma biblioteca para Linux em um sistema nao Linux ***
#endif


UNIV_FILE file_open(const char *path, const int mode) {
    int oflag = 0;
    switch(mode) {
        case M_READ:
            oflag = O_RDONLY;
            break;
        case M_WRITE:
            oflag = O_WRONLY;
            break;
        case M_WRITE_NEW:
            oflag = O_WRONLY | O_CREAT;
            break;
        default:
            fatal(0, "FATAL: Modo %d desconhecido.\n", mode);
    }

    UNIV_FILE file = open(path, oflag);
    if(file < 0)
        fatal(1, "FATAL: Nao foi possivel abrir o arquivo.\n")

    if(chmod(path, 00666) < 0)
        fatal(1, "FATAL: Nao foi possivel colocar permissoes no arquivo.\n")

    return file;
}

SIZ file_getSize(UNIV_FILE file) {
    struct stat info;
    fstat(file, &info);

    SIZ size = info.st_size;
    if(size > MAX_SIZ)
        fatal(0, "FATAL: O tamanho do arquivo ultrapassou o limite.\n")

    return size;
}

bool file_exists(const char *path) {
    struct stat info;
    bool ret = (stat(path, &info) >= 0);
    return ret;
}

void file_read(void *ptr, SIZ size, UNIV_FILE file) {
    if(size > MAX_SIZ)
        fatal(0, "FATAL: O tamanho do arquivo ultrapassou o limite.\n")

    bool ret = (read(file, ptr, size) >= 0);
    if(!ret)
        fatal(1, "FATAL: Nao foi possivel ler os primeiros %u bytes do arquivo.\n", size)
}

void file_write(const void *ptr, SIZ size, UNIV_FILE file) {
    bool ret = (write(file, ptr, size) >= 0);
    if(!ret)
        fatal(1, "FATAL: Nao foi possivel escrever %u bytes no arquivo.\n", size)
}

void file_close(UNIV_FILE file) {
    close(file);
}