#include <assert.h>
#include <stddef.h>
#include <unistd.h>

#define ALIGN(x) (((x) + 7) & ~7)

typedef struct Block {
    size_t size;
    int isFree;

    struct Block *next;

} Block;

// head of the blocks
Block *head = NULL;

// UTILS
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

// block splitting
void block_splitting(Block *block, size_t size) {
    Block *new_block = (Block *)((char *)(block + 1) + size);

    new_block->size = block->size - size - sizeof(Block);
    new_block->next = block->next;
    new_block->isFree = 1;

    block->size = size;
    block->next = new_block;

    return;
}

// coalesce
void coalesce(Block *block) {
    while (block->next) {
        if (block->next->isFree) {
            block->size = sizeof(Block) + block->next->size;
            block->next = block->next->next;
        }
    }
}

// MALLOC
void *my_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }

    Block *block;

    size = ALIGN(size);

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
            if (block->size >= size + sizeof(Block) + 8) {
                block_splitting(block, size);
            }

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

    coalesce(block);

    // backward

    Block *curr = head;

    while (curr->next != NULL) {
        if (curr->next != block) {
            curr = curr->next;
        }
    }

    if (curr != NULL && curr->isFree == 1) {
        coalesce(curr);
    }
}

int main() {
    int *block_1 = my_malloc(200);
    free_my_malloc(block_1);
    int *block_2 = my_malloc(100);
    int *block_3 = my_malloc(50);

    return 0;
}
