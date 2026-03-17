#include <stdio.h>
#include <string.h>

typedef int (*command_fn)(int argc, char *argv[]);

typedef struct {
    const char *name;
    command_fn fn;
} Command;

int cmd_init(int argc, char *argv[]) {
    printf("Initialized empty repository\n");
    return 0;
}

int cmd_hash_object(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: git hash-object <file>\n");
        return 1;
    }

    printf("Hashing file: %s\n", argv[2]);
    return 0;
}

Command commands[] = {
    {"init", cmd_init},
    {"hash-object", cmd_hash_object},
};

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: git <command>\n");
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
