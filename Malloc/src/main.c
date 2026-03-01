#include <assert.h>
#include <stddef.h>
#include <unistd.h>

typedef struct Block {
    size_t size;
    int isFree;

    struct Block *next;

} Block;

// head of the blocks
Block *head = NULL;

// utils
Block *request_space(size_t size) {
    Block *block = sbrk(0);

    void *request = sbrk(sizeof(Block) + size);

    if (request == (void *)-1) {
        return NULL;
    }

    block->size = size;
    block->isFree = 0;
    block->next = NULL;

    return block;
}

// find free block
Block *find_free_block(size_t size) {
    Block *curr = head;

    while (curr != NULL) {
        if (curr->isFree == 1 && curr->size >= size) {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

// malloc
void *my_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    Block *block;

    if (head == NULL) {
        block = request_space(size);

        if (block == NULL) {
            return NULL;
        }

        head = block;
    } else {
        block = find_free_block(size);

        if (block == NULL) {
            Block *curr = head;

            while (curr->next != NULL) {
                curr = curr->next;
            }

            block = request_space(size);

            if (block == NULL) {
                return NULL;
            }

            curr->next = block;
        } else {
            block->isFree = 0;
        }
    }

    return (block + 1);
}

void free_my_malloc(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    Block *block = (Block *)ptr - 1;
    block->isFree = 1;
}

int main() {
    //

    return 0;
}
