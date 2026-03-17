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

int write_file(const char *path, const void *data, size_t size) {
    FILE *f = fopen(path, "wb");
    if (!f)
        return -1;

    fwrite(data, 1, size, f);
    fclose(f);
    return 0;
}

char *read_file_exact(const char *path, size_t *size_out) {
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
