#ifndef TREE_H
#define TREE_H

#include <stddef.h>

#define MODE_FILE 0100644
#define MODE_DIR 0040000

typedef struct {
    char *name;
    unsigned char hash[20];
    int mode;
} TreeEntry;

char *write_tree(TreeEntry *entries, size_t n);
char *write_tree_dir(const char *dirpath);

#endif
