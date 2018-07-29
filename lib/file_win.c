#include "file.h"

#ifndef WINDOWS
 #error *** Voce esta tentando compilar uma biblioteca para Windows em um sistema nao Windows ***
#endif
 
#define SET_MODES(acessValue, shareValue, creationValue) {acess=acessValue; share=shareValue;creation=creationValue;}

UNIV_FILE file_open(const char *path, const int mode) {
    DWORD acess, share, creation;
    switch(mode) {
        case M_READ:
            SET_MODES(GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING)
            break;
        case M_WRITE:
            SET_MODES(GENERIC_WRITE, FILE_SHARE_WRITE, TRUNCATE_EXISTING)
            break;
        case M_WRITE_NEW:
            SET_MODES(GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS)
            break;
        default:
            fatal(0, "FATAL: Modo %d desconhecido.\n", mode)
    }

    UNIV_FILE file = CreateFile(path, acess, share, NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE) 
        fatal(1, "FATAL: Nao foi possivel abrir o arquivo.\n")

    return file;
}

SIZ file_getSize(UNIV_FILE file) {
    LARGE_INTEGER info;
    GetFileSizeEx(file, &info);

    long tempSize = info.QuadPart;
    if(tempSize > MAX_SIZ) 
        fatal(0, "FATAL: O tamanho do arquivo ultrapassou o limite.\n")
    
    SIZ size = tempSize;
    return size;
}

bool file_exists(const char *path) {
    WIN32_FIND_DATA info;
    bool ret = (FindFirstFile(path, &info) != INVALID_HANDLE_VALUE);

    return ret;
}

void file_read(void *ptr, SIZ size, UNIV_FILE file) {
    if(size > MAX_SIZ)
        fatal(0, "FATAL: O tamanho do arquivo ultrapassou o limite.\n")

    bool ret = ReadFile(file, ptr, size, NULL, NULL);
    if(!ret) 
        fatal(1, "FATAL: Nao foi possivel ler os primeiros %zu bytes do arquivo.\n", size)
}

void file_write(const void *ptr, SIZ size, UNIV_FILE file) {
    bool ret = WriteFile(file, ptr, size, NULL, NULL);
    if(!ret)
        fatal(1, "FATAL: Nao foi possivel escrever %zu bytes no arquivo.\n", size)
}

void file_close(UNIV_FILE file) {
    CloseHandle(file);
}
