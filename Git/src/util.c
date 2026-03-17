#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

char *read_file(const char *path, size_t *size_out) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size);
    fread(buffer, 1, size, f);
    fclose(f);

    *size_out = size;
    return buffer;
}

int create_dir(const char *path) {
    //
    return mkdir(path, 0755);
}
