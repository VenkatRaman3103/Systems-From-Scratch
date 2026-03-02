#define _DEFAULT_SOURCE
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ALIGN(x) (((x) + 7) & ~7)

typedef struct Block {
    size_t size;
    int isFree;

    struct Block *next;

} Block;

void free_my_malloc(void *ptr);

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
    while (block->next && block->next->isFree) {
        block->size += sizeof(Block) + block->next->size;
        block->next = block->next->next;
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

// REALLOC
void *my_realloc(void *ptr, size_t new_size) {
    if (ptr == NULL) {
        return my_malloc(new_size);
    }

    if (new_size == 0) {
        free_my_malloc(ptr);
        return NULL;
    }

    Block *block = (Block *)ptr - 1;

    // shrink
    if (block->size >= new_size) {
        if (block->size >= new_size + sizeof(Block) + 8) {
            block_splitting(block, new_size);
        }

        return ptr;
    }

    // grow
    // merge with next block if possible
    Block *nextBlock = block->next;

    if (nextBlock && nextBlock->isFree == 1 &&
        block->size + sizeof(Block) + nextBlock->size >= new_size) {

        coalesce(block);

        if (block->size >= new_size + sizeof(Block) + 8) {
            block_splitting(block, new_size);
        }

        return ptr;
    }

    // creat a new block and copy the content
    void *new_ptr = my_malloc(new_size);
    if (!new_ptr) {
        return NULL;
    }

    size_t copy_size = block->size < new_size ? block->size : new_size;
    memcpy(new_ptr, ptr, copy_size);

    free_my_malloc(ptr);

    return new_ptr;
}

// free
void free_my_malloc(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    Block *block = (Block *)ptr - 1;
    block->isFree = 1;

    coalesce(block);

    // backward
    Block *curr = head;

    while (curr->next != NULL && curr->next != block) {
        curr = curr->next;
    }

    if (curr != NULL && curr->isFree == 1) {
        coalesce(curr);
    }
}

void print_heap() {
    Block *curr = head;
    printf("Heap blocks:\n");
    while (curr) {
        printf(" [%s %zu]", curr->isFree ? "free" : "used", curr->size);
        curr = curr->next;
    }
    printf("\n");
}

int main() {
    int *a = my_malloc(200);
    print_heap();

    int *b = my_malloc(100);
    print_heap();

    int *c = my_malloc(50);
    print_heap();

    printf("\nFreeing b...\n");
    free_my_malloc(b);
    print_heap();

    printf("\nRealloc c to 120 (grow)...\n");
    c = my_realloc(c, 120);
    print_heap();

    printf("\nRealloc a to 100 (shrink)...\n");
    a = my_realloc(a, 100);
    print_heap();

    printf("\nAllocating d = 80...\n");
    int *d = my_malloc(80);
    print_heap();

    printf("\nFreeing all...\n");
    free_my_malloc(a);
    free_my_malloc(c);
    free_my_malloc(d);
    print_heap();

    return 0;
}
