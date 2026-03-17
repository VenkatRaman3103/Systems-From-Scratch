#include <stdio.h>
#include <string.h>

void cmd_init() {
    printf("initialize\n");
    return;
}

void cmd_hash_object(char *filename) {
    printf("%s\n", filename);
    return;
}

int main(int argc, char *argv[]) {
    if (strcmp(argv[1], "init") == 0) {
        cmd_init();
    } else if (strcmp(argv[1], "hash-object") == 0) {
        if (argc < 3) {
            printf("Usage: git hash-object <file>\n");
            return 1;
        }

        cmd_hash_object(argv[2]);
    } else {
        printf("unknown command");
    }

    return 0;
}
