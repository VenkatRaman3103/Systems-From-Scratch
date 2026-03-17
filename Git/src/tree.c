
#define _POSIX_C_SOURCE 200809L

#include "../include/tree.h"
#include "../include/hash.h"
#include "../include/util.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef MODE_FILE
#endif

// write a tree object from an array of TreeEntry
char *write_tree(TreeEntry *entries, size_t n) {
    unsigned char hash[20];

    // serialize entries
    size_t total_len = 0;
    for (size_t i = 0; i < n; i++) {
        total_len += strlen(entries[i].name) + 40 +
                     6; // name + hash hex + mode+space+null
    }

    char *buffer = malloc(total_len);
    if (!buffer)
        return NULL;

    char *p = buffer;
    for (size_t i = 0; i < n; i++) {
        int written = sprintf(p, "%o %s", entries[i].mode, entries[i].name);
        p += written;
        hash_to_hex(entries[i].hash, p);
        p += 40;
        *p++ = '\n';
    }

    sha1_hash((unsigned char *)buffer, p - buffer, hash);

    char hex[41];
    hash_to_hex(hash, hex);
    hex[40] = '\0';

    // object path
    char path[256];
    snprintf(path, sizeof(path), ".mygit/objects/%c%c", hex[0], hex[1]);
    mkdir(path, 0755);

    char fullpath[300];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, hex + 2);

    // write tree object
    if (write_file(fullpath, (unsigned char *)buffer, p - buffer) != 0) {
        free(buffer);
        return NULL;
    }

    free(buffer);
    return strdup(hex);
}

// recursively write tree for directory
char *write_tree_dir(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir)
        return NULL;

    TreeEntry entries[1024]; // max 1024 entries
    size_t n = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (n >= 1024)
            break;

        entries[n].name = strdup(entry->d_name);
        entries[n].mode = MODE_FILE;

        unsigned char hash[20];
        size_t size;
        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);

        char *content = read_file(filepath, &size);
        if (!content)
            continue;

        char header[64];
        int header_len = sprintf(header, "blob %zu", size) + 1;
        size_t total_size = header_len + size;
        unsigned char *store = malloc(total_size);
        memcpy(store, header, header_len);
        memcpy(store + header_len, content, size);

        sha1_hash(store, total_size, hash);
        memcpy(entries[n].hash, hash, 20);

        free(content);
        free(store);

        n++;
    }

    closedir(dir);

    return write_tree(entries, n);
}
