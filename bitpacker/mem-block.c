
#include <memory.h>
#include <stdio.h>

#include "mem-block.h"

struct mem_block_data {
    char    *data_;
    size_t   capacity_;
    size_t   used_;
};

typedef struct mem_block_data mem_block_data_type;

static const size_t void_ptr_size      = sizeof(void *);
static const size_t void_ptr_size_mask = sizeof(void *) - 1;

#define mem_block_fix_size( new_size )                             \
    ( new_size <= void_ptr_size)                                   \
      ? void_ptr_size                                              \
      : (new_size + ((new_size-void_ptr_size) & void_ptr_size_mask))

#if 0
size_t mem_block_fix_size( size_t new_size )
{
    static const size_t void_ptr_size = sizeof(void *);

    if( 0 == new_size ) return void_ptr_size;
    new_size = ((new_size-1)/sizeof(void_ptr_size)*sizeof(void_ptr_size))
                                                  +sizeof(void_ptr_size);
    return new_size;
}
#endif

int mem_block_reserve(struct mem_block_data *mb, size_t new_size)
{
    static int i=0;

    size_t old_capa = mb->capacity_;
    size_t new_capa = mem_block_fix_size(old_capa + (old_capa >> 1)); // * 1.5

    if( new_size <= mb->capacity_ ) return 1;

    new_size = mem_block_fix_size(new_size);

    if( new_capa > new_size ) new_size = new_capa;

    printf("reallock %d %d\n", i++, new_size);

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

int mem_block_resize2(struct mem_block_data *mb, size_t new_size, int c)
{
    size_t old_used = mb->used_;

    if( 0 == mem_block_reserve( mb, new_size ) )
        return 0;

    mb->used_ = new_size;
    if( old_used < new_size ) {
        memset( &mb->data_[old_used], c, new_size - old_used );
    }
    return 1;
}

int mem_block_resize(struct mem_block_data *mb, size_t new_size)
{
    return mem_block_resize2( mb, new_size, 0 );
}

mem_block_data_type *mem_block_new( size_t init_size )
{
    mem_block_data_type *new_block =
            (mem_block_data_type *)malloc(sizeof(mem_block_data_type));

    size_t new_size = mem_block_fix_size(init_size);

    new_block->data_     = (char *)malloc(new_size);
    if( NULL == new_block->data_ ) {
        free(new_block);
        return NULL;
    }

    new_block->capacity_ = new_size;
    new_block->used_     = init_size;

    if( 0 != init_size )
        memset( new_block->data_, 0, init_size );

    return new_block;
}

int mem_block_free(mem_block_data_type *mb)
{
    if( NULL == mb ) return 1;
    free(mb->data_);
    free(mb);
    return 1;
}

size_t mem_block_size(mem_block_data_type *mb)
{
    return mb->used_;
}

size_t mem_block_capacity(struct mem_block_data *mb)
{
    return mb->capacity_;
}

size_t mem_block_available(struct mem_block_data *mb)
{
    return (mb->capacity_ - mb->used_);
}

void*  mem_block_data(struct mem_block_data *mb)
{
    return mb->data_;
}


int mem_block_clear(struct mem_block_data *mb)
{
    mb->used_ = 0;
    return 1;
}

int mem_block_swap(struct mem_block_data *lmb, struct mem_block_data *rmb)
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

    return 1;
}

int mem_block_concat(struct mem_block_data *lmb, const void *data, size_t len)
{
    size_t old_used = lmb->used_;
    if( 0 == mem_block_resize2(lmb, old_used + len, 0) ) return 0;
    memcpy( &lmb->data_[old_used], data, len );
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


