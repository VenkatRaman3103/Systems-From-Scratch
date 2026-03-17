#define _POSIX_C_SOURCE 200809L
#include "../include/hash.h"
#include "../include/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

char *commit_tree(const char *tree_hash, const char *message) {
    // build commit content
    char content[1024];
    int len = snprintf(content, sizeof(content),
                       "tree %s\n"
                       "author you <you@example.com>\n"
                       "\n"
                       "%s\n",
                       tree_hash, message);

    // build object: "commit <size>\0<content>"
    char header[64];
    int header_len = snprintf(header, sizeof(header), "commit %d", len) + 1;

    int total_size = header_len + len;
    unsigned char *store = malloc(total_size);

    memcpy(store, header, header_len);
    memcpy(store + header_len, content, len);

    // hash
    unsigned char hash[20];
    sha1_hash(store, total_size, hash);

    char hex[41];
    hash_to_hex(hash, hex);
    hex[40] = '\0';

    // split hash
    char dir[3];
    strncpy(dir, hex, 2);
    dir[2] = '\0';

    const char *file = hex + 2;

    // create object directory
    char path[256];
    snprintf(path, sizeof(path), ".mygit/objects/%s", dir);
    mkdir(path, 0755);

    // full path
    char fullpath[300];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, file);

    // write object
    if (write_file(fullpath, store, total_size) != 0) {
        perror("write_file");
        free(store);
        return NULL;
    }

    free(store);
    return strdup(hex);
}
