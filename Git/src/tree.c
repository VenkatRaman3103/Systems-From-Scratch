#include "../include/tree.h"
#include "../include/hash.h"
#include "../include/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// helper: store object in .mygit/objects
static int store_object(const unsigned char *data, size_t size,
                        unsigned char *out_hash) {
    // hash the object
    sha1_hash(data, size, out_hash);

    char hex[41];
    hash_to_hex(out_hash, hex);
    hex[40] = '\0';

    char dir[3];
    strncpy(dir, hex, 2);
    dir[2] = '\0';
    const char *file = hex + 2;

    // create object directory
    char path[256];
    sprintf(path, ".mygit/objects/%s", dir);
    mkdir(path, 0755);

    char fullpath[300];
    sprintf(fullpath, "%s/%s", path, file);

    return write_file(fullpath, data, size);
}

// write_tree: serialize TreeEntry array and store as tree object
char *write_tree(TreeEntry *entries, size_t n) {
    // calculate total size
    size_t tree_size = 0;
    for (size_t i = 0; i < n; i++) {
        tree_size +=
            snprintf(NULL, 0, "%o %s", entries[i].mode, entries[i].name) + 1 +
            20;
    }

    unsigned char *buffer = malloc(tree_size);
    size_t offset = 0;

    for (size_t i = 0; i < n; i++) {
        int len = sprintf((char *)buffer + offset, "%o %s", entries[i].mode,
                          entries[i].name);
        offset += len;
        buffer[offset++] = '\0';
        memcpy(buffer + offset, entries[i].hash, 20);
        offset += 20;
    }

    // prepend tree header
    char header[64];
    int header_len = sprintf(header, "tree %zu", tree_size) + 1;
    size_t total_size = header_len + tree_size;
    unsigned char *store = malloc(total_size);
    memcpy(store, header, header_len);
    memcpy(store + header_len, buffer, tree_size);

    free(buffer);

    // store object
    unsigned char hash[20];
    if (store_object(store, total_size, hash) != 0) {
        perror("store tree object");
        free(store);
        return NULL;
    }

    free(store);

    // return hex string of hash
    char *hex = malloc(41);
    hash_to_hex(hash, hex);
    hex[40] = '\0';
    return hex;
}
