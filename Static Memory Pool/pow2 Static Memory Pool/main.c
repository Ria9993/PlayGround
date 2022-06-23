#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

typedef struct {
	int32_t value;
} data_t;

typedef struct {
	int8_t head;
	//byte padding
	data_t data;
} block_t;

typedef struct {
	block_t* memory_pool;
	int* index_table;
	int index_table_i;
} chunk_t;

void chunk_init(chunk_t* const chunk, const int size) {
	chunk->memory_pool = (block_t*)malloc(size * sizeof(block_t));
	chunk->index_table = (int*)malloc(size * sizeof(int));
	chunk->index_table_i = 0;
	for (int i = 0; i < size; i++) {
		chunk->index_table[i] = i;
	}
}

void chunk_destroy(chunk_t* const chunk) {
	free(chunk->memory_pool);
	free(chunk->index_table);
}

typedef struct {
	chunk_t* chunks;
	int chunk_size;
	int chunk_count;
	int chunk_capacity;
} fixed_memory_pool_t;

void pool_init(fixed_memory_pool_t* const pool, const int chunk_size) {
	pool->chunk_capacity = 1;
	pool->chunks = (chunk_t*)malloc(1 * sizeof(chunk_t));
	chunk_init(&pool->chunks[0], chunk_size);
	pool->chunk_count = 1;
	pool->chunk_size = chunk_size;
}

void pool_destroy(fixed_memory_pool_t* const pool) {
	for (int i = 0; i < pool->chunk_count; ++i) {
		chunk_destroy(&pool->chunks[i]);
	}
	free(pool->chunks);
}

data_t* data_alloc(fixed_memory_pool_t* const pool) {
	int chunk_i;

	// find remainig chunk
	for (chunk_i = 0; chunk_i < pool->chunk_count; ++chunk_i) {
		if (pool->chunks[chunk_i].index_table_i < pool->chunk_size) {
			goto block_alloc;
		}
	}

	// new chunk
	if (pool->chunk_count >= pool->chunk_capacity) {
		pool->chunk_capacity <<= 1;
		pool->chunks = (chunk_t*)realloc(pool->chunks, pool->chunk_capacity * sizeof(chunk_t));
	}
	chunk_init(&pool->chunks[pool->chunk_count++], pool->chunk_size);

block_alloc:
	chunk_t* const chunk = &pool->chunks[chunk_i];
	block_t* const block = chunk->memory_pool + chunk->index_table[chunk->index_table_i++];
	block->head = chunk_i;
	return &block->data;
}

void data_free(fixed_memory_pool_t* const pool, data_t* const address) {
	const int head_size = sizeof(block_t) - sizeof(data_t);
	block_t* const block = (block_t*)((char*)address - head_size);
	chunk_t* const chunk = &pool->chunks[block->head];
	const int idx = (int)(block - chunk->memory_pool);
	chunk->index_table[--chunk->index_table_i] = idx;
}

int main() {
	fixed_memory_pool_t pool;
	data_t* tmp[16];
	enum { CHUNK_SIZE = 3 };

	pool_init(&pool, CHUNK_SIZE);
	{
		tmp[0] = data_alloc(&pool); //maybe chunk #1
		tmp[1] = data_alloc(&pool);
		tmp[2] = data_alloc(&pool);
		tmp[3] = data_alloc(&pool); //maybe chunk #2
		tmp[4] = data_alloc(&pool);
		tmp[5] = data_alloc(&pool);

		data_free(&pool, tmp[0]);
		data_free(&pool, tmp[3]);

		assert(tmp[0] == data_alloc(&pool));
		assert(tmp[3] == data_alloc(&pool));
	}
	pool_destroy(&pool);

	// Check MemoryLeaks
	_CrtDumpMemoryLeaks(); //None detectecd

	return 0;
}