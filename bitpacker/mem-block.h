#if !defined(_MEM_BLOCK_20131026_H_)
#define _MEM_BLOCK_20131026_H_

#include <stdlib.h>

struct mem_block_data;

struct mem_block_data *mem_block_new( );
void mem_block_delete(struct mem_block_data *mb);
size_t mem_block_size(struct mem_block_data *mb);
int    mem_block_resize (struct mem_block_data *mb, size_t new_size);
int    mem_block_reserve(struct mem_block_data *mb, size_t new_size);
int    mem_block_clear(struct mem_block_data *mb);
void   mem_block_swap(struct mem_block_data *lmb, struct mem_block_data *rmb);

int mem_block_concat(struct mem_block_data *mb,
                     const char *data, size_t len);
int mem_block_concat2(struct mem_block_data *mb,
                      const struct mem_block_data *cmb);

#endif
