#define _POSIX_C_SOURCE 200809L

#include "../include/tree.h"
#include "../include/hash.h"
#include "../include/util.h"

#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


// simple strdup replacement
static char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy)
        memcpy(copy, s, len);
    return copy;
}

// write a tree object from an array of TreeEntry
char *write_tree(TreeEntry *entries, size_t n) {
    unsigned char hash[20];

    size_t entries_len = 0;
    for (size_t i = 0; i < n; i++) {
        entries_len += strlen(entries[i].name) + 40 + 10;
    }

    char header[64];
    int header_len = sprintf(header, "tree %zu", entries_len) + 1;
    size_t total_len = header_len + entries_len;

    char *buffer = malloc(total_len);
    if (!buffer)
        return NULL;

    memcpy(buffer, header, header_len);
    char *p = buffer + header_len;

    for (size_t i = 0; i < n; i++) {
        int written = snprintf(p, total_len - (p - buffer), "%o %s ",
                               entries[i].mode, entries[i].name);
        p += written;

        hash_to_hex(entries[i].hash, p);
        p += 40;

        *p++ = '\n';
    }

    size_t final_size = p - buffer;
    sha1_hash((unsigned char *)buffer, final_size, hash);

    char hex[41];
    hash_to_hex(hash, hex);
    hex[40] = '\0';

    char path[256];
    snprintf(path, sizeof(path), ".mygit/objects/%c%c", hex[0], hex[1]);
    mkdir(path, 0755);

    char fullpath[300];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, hex + 2);

    if (write_file(fullpath, (unsigned char *)buffer, final_size) != 0) {
        free(buffer);
        return NULL;
    }

    free(buffer);
    return my_strdup(hex);
}

// recursively write tree for directory
char *write_tree_dir(const char *dirpath) {
    DIR *dir = opendir(dirpath);
    if (!dir)
        return NULL;

    TreeEntry entries[1024];
    size_t n = 0;

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0 ||
            strcmp(entry->d_name, ".mygit") == 0)
            continue;

        if (n >= 1024)
            break;

        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);

        struct stat st;
        if (stat(filepath, &st) != 0)
            continue;

        entries[n].name = my_strdup(entry->d_name);

        // Directory
        if (S_ISDIR(st.st_mode)) {
            entries[n].mode = MODE_DIR;

            char *subtree_hash = write_tree_dir(filepath);
            if (!subtree_hash) {
                free(entries[n].name);
                continue;
            }

            hex_to_hash(subtree_hash, entries[n].hash);
            free(subtree_hash);
        }
        // Files
        else if (S_ISREG(st.st_mode)) {
            entries[n].mode = MODE_FILE;

            size_t size;
            char *content = read_file(filepath, &size);
            if (!content) {
                perror("read_file");
                free(entries[n].name);
                continue;
            }

            char header[64];
            int header_len =
                snprintf(header, sizeof(header), "blob %zu", size) + 1;

            size_t total_size = header_len + size;
            unsigned char *store = malloc(total_size);

            memcpy(store, header, header_len);
            memcpy(store + header_len, content, size);

            unsigned char hash[20];
            sha1_hash(store, total_size, hash);

            char file_hex[41];
            hash_to_hex(hash, file_hex);
            file_hex[40] = '\0';

            char obj_dir[256];
            snprintf(obj_dir, sizeof(obj_dir), ".mygit/objects/%c%c",
                     file_hex[0], file_hex[1]);
            mkdir(obj_dir, 0755);

            char obj_path[300];
            snprintf(obj_path, sizeof(obj_path), "%s/%s", obj_dir,
                     file_hex + 2);
            write_file(obj_path, store, total_size);

            memcpy(entries[n].hash, hash, 20);

            free(content);
            free(store);
        } else {
            // skip symlinks, sockets, etc
            free(entries[n].name);
            continue;
        }

        n++;
    }

    closedir(dir);

    return write_tree(entries, n);
}
