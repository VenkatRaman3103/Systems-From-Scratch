#ifndef TREE_H
#define TREE_H

#include <stddef.h>

#define MODE_FILE 100644
#define MODE_DIR 40000

typedef struct {
    char *name;
    unsigned char hash[20];
    int mode;
} TreeEntry;

char *write_tree(TreeEntry *entries, size_t n); // returns hash as hex

#endif
