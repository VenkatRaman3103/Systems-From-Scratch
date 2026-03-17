#include "../include/commit.h"
#include "../include/hash.h"
#include "../include/tree.h"
#include "../include/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef int (*command_fn)(int argc, char *argv[]);

typedef struct {
    const char *name;
    command_fn fn;
} Command;

int cmd_commit_tree(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: mygit commit-tree <tree_hash> -m <message>\n");
        return 1;
    }

    const char *tree_hash = argv[2];
    const char *message = NULL;

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            message = argv[i + 1];
            break;
        }
    }

    if (!message) {
        printf("Commit message required (-m)\n");
        return 1;
    }

    char *commit_hash = commit_tree(tree_hash, message);
    if (!commit_hash)
        return 1;

    printf("%s\n", commit_hash);
    free(commit_hash);
    return 0;
}

int cmd_write_tree(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    char *tree_hex = write_tree_dir(".");
    if (!tree_hex)
        return 1;

    printf("%s\n", tree_hex);
    free(tree_hex);
    return 0;
}

int cmd_cat_file(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: git cat-file <hash>\n");
        return 1;
    }

    const char *hash = argv[2];

    // Split hash
    char dir[3];
    strncpy(dir, hash, 2);
    dir[2] = '\0';

    const char *file = hash + 2;

    // Build path
    char path[300];
    sprintf(path, ".mygit/objects/%s/%s", dir, file);

    size_t size;
    char *data = read_file_exact(path, &size);
    if (!data) {
        perror("read object");
        return 1;
    }

    // Find null separator
    char *content = memchr(data, '\0', size);
    if (!content) {
        printf("Invalid object format\n");
        free(data);
        return 1;
    }

    content++; // move past '\0'

    size_t content_size = size - (content - data);

    fwrite(content, 1, content_size, stdout);
    printf("\n");

    free(data);
    return 0;
}

int cmd_init(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    create_dir(".mygit");
    create_dir(".mygit/objects");
    create_dir(".mygit/refs");

    FILE *head = fopen(".mygit/HEAD", "w");
    if (!head) {
        perror("HEAD");
        return 1;
    }

    fprintf(head, "ref: refs/heads/main\n");
    fclose(head);

    printf("Initialized empty Git repository\n");
    return 0;
}

int cmd_hash_object(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: git hash-object <file>\n");
        return 1;
    }

    size_t size;
    char *content = read_file(argv[2], &size);
    if (!content) {
        perror("read_file");
        return 1;
    }

    // Build Git blob: "blob <size>\0<data>"
    char header[64];
    int header_len = sprintf(header, "blob %zu", size) + 1;

    size_t total_size = header_len + size;
    unsigned char *store = malloc(total_size);

    memcpy(store, header, header_len);
    memcpy(store + header_len, content, size);

    // Hash
    unsigned char hash[20];
    sha1_hash(store, total_size, hash);

    char hex[41];
    hash_to_hex(hash, hex);
    hex[40] = '\0';

    // Split hash
    char dir[3];
    strncpy(dir, hex, 2);
    dir[2] = '\0';

    const char *file = hex + 2;

    // Create directory path
    char path[256];
    sprintf(path, ".mygit/objects/%s", dir);
    mkdir(path, 0755);

    // Full file path
    char fullpath[300];
    sprintf(fullpath, "%s/%s", path, file);

    // Write object
    if (write_file(fullpath, store, total_size) != 0) {
        perror("write_file");
        free(content);
        free(store);
        return 1;
    }

    printf("%s\n", hex);

    free(content);
    free(store);
    return 0;
}

Command commands[] = {
    {"init", cmd_init},
    {"hash-object", cmd_hash_object},
    {"cat-file", cmd_cat_file},
    {"write-tree", cmd_write_tree},
    {"commit-tree", cmd_commit_tree},
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: mygit <command>\n");
        return 1;
    }

    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (strcmp(argv[1], commands[i].name) == 0) {
            return commands[i].fn(argc, argv);
        }
    }

    printf("Unknown command: %s\n", argv[1]);
    return 1;
}
