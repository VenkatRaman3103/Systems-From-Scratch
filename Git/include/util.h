#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

char *read_file(const char *path, size_t *size_out);
int create_dir(const char *path);

#endif
