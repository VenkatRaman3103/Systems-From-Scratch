#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

char *read_file(const char *path, size_t *size_out);
int create_dir(const char *path);
int write_file(const char *path, const void *data, size_t size);
char *read_file_exact(const char *path, size_t *size_out);

#endif
