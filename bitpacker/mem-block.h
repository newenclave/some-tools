#if !defined(_MEM_BLOCK_20131026_H_)
#define _MEM_BLOCK_20131026_H_

#include <stdlib.h>

struct mem_block_data;

struct mem_block_data *mem_block_new( size_t init_size );

int mem_block_free(struct mem_block_data *mb);

size_t mem_block_size(struct mem_block_data *mb);
size_t mem_block_capacity(struct mem_block_data *mb);
void*  mem_block_data(struct mem_block_data *mb);

int mem_block_resize(struct mem_block_data *mb, size_t new_size);
int mem_block_resize2(struct mem_block_data *mb, size_t new_size, int c);
int mem_block_reserve(struct mem_block_data *mb, size_t new_size);
int mem_block_clear(struct mem_block_data *mb);
int mem_block_swap(struct mem_block_data *lmb, struct mem_block_data *rmb);

int mem_block_concat(struct mem_block_data *mb,
                     const void *data, size_t len);
int mem_block_concat2(struct mem_block_data *mb,
                      const struct mem_block_data *cmb);

int mem_block_push_back(struct mem_block_data *mb, char c);

#endif