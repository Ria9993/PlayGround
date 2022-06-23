#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

int32_t* memory_pool; //fixed_size memory pool
int* index_table;
int index_table_i;
int max_size;

void init_(int max_size0) {
    max_size = max_size0;
    memory_pool = (int32_t*)malloc(max_size * sizeof(int32_t));
    index_table = (int32_t*)malloc(max_size * sizeof(int32_t));
    index_table_i = 0;
    for (int i = 0; i < max_size; ++i) {
        index_table[i] = i;
    }
}

int32_t* alloc_() {
    assert(index_table_i <= max_size);
    return memory_pool + index_table[index_table_i++];
}

void free_(int32_t* address) {
    assert(index_table_i >= 0);
    index_table[--index_table_i] = (int)(address - memory_pool);
}

void destroy_() {
    free(memory_pool);
    free(index_table);
}

int main() {
    enum { MAX_SIZE = 16 };
    int32_t* tmp[MAX_SIZE];

    init_(MAX_SIZE);
    {
        tmp[0] = alloc_();
        tmp[1] = alloc_();
        tmp[2] = alloc_();
        tmp[3] = alloc_();

        free_(tmp[0]);
        free_(tmp[2]);

        assert(tmp[2] == alloc_());
        assert(tmp[0] == alloc_());
    }
    destroy_();

    return 0;
}