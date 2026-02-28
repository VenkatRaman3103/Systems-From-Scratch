#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_CAPACITY 640000 // upper bound

char heap[HEAP_CAPACITY] = {0};

size_t heap_size = 0;

// malloc(size_t <size>)
void *heap_alloc(size_t size) {

    assert((heap_size + size) <= HEAP_CAPACITY);
    return NULL;
}

void rough() {
    int window = 2;

    char string[] = "Hello world";
    int stringLength = sizeof(string);
    printf("%d\n", stringLength);

    for (int i = 0; i < stringLength - window; i++) {
        printf("%c\n", string[i]);

        for (int j = i; j < i + window; j++) {
            printf("%c", string[j]);
        }

        printf("\n");
    }
}

int main() {

    heap_alloc(20);

    rough();

    return 0;
}
