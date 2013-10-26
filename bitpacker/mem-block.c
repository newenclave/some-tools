
#include <memory.h>
#include <stdio.h>

#include "mem-block.h"

struct mem_block_data {
    char    *data_;
    size_t   capacity_;
    size_t   used_;
};

typedef struct mem_block_data mem_block_data_type;

int mem_block_reserve(struct mem_block_data *mb, size_t new_size)
{
    static const size_t void_ptr_size = sizeof(void *);

    if( new_size <= mb->capacity_ ) return 1;

    new_size = ((new_size-1)/sizeof(void_ptr_size)*sizeof(void_ptr_size))
                                                  +sizeof(void_ptr_size);

    char *new_data = (char *)realloc(mb->data_, new_size);
    if( NULL == new_data ) {
        printf( "reserve fail\n");
        return 0;
    } else {
        mb->data_     = new_data;
        mb->capacity_ = new_size;
    }
    return 1;
}

int mem_block_resize(struct mem_block_data *mb, size_t new_size)
{
    size_t old_used = mb->used_;

    printf( " old block: %d", old_used );
    printf( " new block: %d", new_size );

    if( 0 == mem_block_reserve( mb, new_size ) )
        return 0;

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
    if( NULL == mb ) return;
    free(mb->data_);
    free(mb);
}

size_t mem_block_size(mem_block_data_type *mb)
{
    return mb->used_;
}

size_t mem_block_capacity(struct mem_block_data *mb)
{
    return mb->capacity_;
}

void*  mem_block_data(struct mem_block_data *mb)
{
    return mb->data_;
}

void mem_block_clear(struct mem_block_data *mb)
{
    mb->used_ = 0;
}

void mem_block_swap(struct mem_block_data *lmb, struct mem_block_data *rmb)
{
    char   *tmp_data = lmb->data_;
    size_t  tmp_used = lmb->used_;
    size_t  tmp_capa = lmb->capacity_;

    lmb->data_ = rmb->data_ ;
    rmb->data_ = tmp_data;

    lmb->used_ = rmb->used_ ;
    rmb->used_ = tmp_used;

    lmb->capacity_ = rmb->capacity_;
    rmb->capacity_ = tmp_capa;
}

int mem_block_concat(struct mem_block_data *lmb, const void *data, size_t len)
{
    if( 0 == mem_block_resize(lmb, lmb->used_ + len) ) return 0;
    memcpy( &lmb->data_[lmb->used_], data, len );
    return 1;
}

int mem_block_concat2(struct mem_block_data *lmb, const struct mem_block_data *rmb)
{
    return mem_block_concat(lmb, rmb->data_, rmb->used_);
}

int mem_block_push_back(struct mem_block_data *mb, char c)
{
    size_t old_capa = mb->capacity_;
    if( ( mb->used_ ) == old_capa ) {
        size_t new_capa = old_capa + (old_capa >> 1); // * 1.5
        if( 0 == mem_block_reserve( mb, new_capa ) )
            return 0;
    }
    mb->data_[mb->used_++] = c;
    return 1;
}


