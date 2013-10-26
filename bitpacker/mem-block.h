#if !defined(_MEM_BLOCK_20131026_H_)
#define _MEM_BLOCK_20131026_H_

#include <stdlib.h>

struct mem_block_data;

struct mem_block_data *mem_block_new( );
void mem_block_delete(struct mem_block_data *mb);
size_t mem_block_size(struct mem_block_data *mb);
int    mem_block_resize(struct mem_block_data *mb, size_t new_size);

#endif
