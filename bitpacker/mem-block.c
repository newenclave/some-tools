
#include <memory.h>
#include <stdio.h>

#include "mem-block.h"

struct mem_block_data {
    char    *data_;
    size_t   capacity_;
    size_t   used_;
};

typedef struct mem_block_data mem_block_data_type;

int mem_block_resize(struct mem_block_data *mb, size_t new_size)
{
    new_size = ((new_size-1)/sizeof(new_size)*sizeof(new_size))
                                             +sizeof(new_size);
    size_t old_used = mb->used_;

    printf( " old block: %d", old_used );
    printf( " new block: %d", new_size );

    if( new_size > mb->capacity_ ) {
        char *new_data = (char *)realloc(mb->data_, new_size);
        if( NULL == new_data ) {
            printf( " fail\n");
            return 0;
        } else {
            mb->data_     = new_data;
            mb->capacity_ = new_size;
        }
    }

    mb->used_ = new_size;
    if( old_used < new_size ) {
        memset( &mb->data_[old_used], 0, new_size - old_used );
        printf( " shift block: %d", new_size - old_used );
    }
    printf("\n");
    return 1;
}

mem_block_data_type *mem_block_new( )
{
    mem_block_data_type *new_block =
            (mem_block_data_type *)malloc(sizeof(mem_block_data_type));

    new_block->data_     = (char *)malloc(sizeof(size_t));
    if( NULL == new_block->data_ ) {
        free(new_block);
        return NULL;
    }
    new_block->capacity_ = sizeof(size_t);
    new_block->used_     = 0;

    return new_block;
}

void mem_block_delete(mem_block_data_type *mb)
{
    free(mb);
}

size_t mem_block_size(mem_block_data_type *mb)
{
    return mb->used_;
}

