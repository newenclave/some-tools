
#include <memory.h>
#include <stdio.h>

#include "mm-block.h"

struct mm_block_data {
    char    *data_;
    size_t   capacity_;
    size_t   used_;
};

typedef struct mm_block_data mm_block_data_type;

static const size_t void_ptr_size      = sizeof(void *);
static const size_t void_ptr_size_mask = sizeof(void *) - 1;

#define mm_block_def_inc(size) (size + (size >> 1))

#define mm_block_fix_size( new_size )                               \
    ( new_size <= void_ptr_size)                                    \
      ? void_ptr_size                                               \
      : (new_size + ((new_size-void_ptr_size) & void_ptr_size_mask))

#if 0
size_t mm_block_fix_size( size_t new_size )
{
    static const size_t void_ptr_size = sizeof(void *);

    if( 0 == new_size ) return void_ptr_size;
    new_size = ((new_size-1)/sizeof(void_ptr_size)*sizeof(void_ptr_size))
                                                  +sizeof(void_ptr_size);
    return new_size;
}
#endif


/// mm_block_new
/// creating new memory block
mm_block_data_type *mm_block_new( size_t init_size )
{
    mm_block_data_type *new_block =
            (mm_block_data_type *)malloc(sizeof(mm_block_data_type));

    size_t new_size = mm_block_fix_size(init_size);

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

struct mm_block_data *mm_block_new_copy( const struct mm_block_data *oth )
{
    size_t new_size = oth->used_;
    struct mm_block_data *new_block = mm_block_new(new_size);
    if( NULL == new_block ) {
        return NULL;
    }
    if( new_size > 0 )
        memcpy( new_block->data_, oth->data_, new_size );
    return new_block;
}


/// mm_block_free
/// free memory
void mm_block_free(mm_block_data_type *mb)
{
    if( NULL != mb ) {
        free(mb->data_);
        free(mb);
    }
}


int mm_block_reserve(struct mm_block_data *mb, size_t new_size)
{
    static int i=0;

    size_t old_capa;
    size_t new_capa;

    if( new_size <= mb->capacity_ ) return 1;

    old_capa = mb->capacity_;
    new_capa = mm_block_fix_size(mm_block_def_inc(old_capa));
    new_size = mm_block_fix_size(new_size);

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

int mm_block_resize2(struct mm_block_data *mb, size_t new_size, int c)
{
    size_t old_used = mb->used_;

    if( 0 == mm_block_reserve( mb, new_size ) )
        return 0;

    mb->used_ = new_size;
    if( old_used < new_size ) {
        memset( &mb->data_[old_used], c, new_size - old_used );
    }
    return 1;
}

int mm_block_resize(struct mm_block_data *mb, size_t new_size)
{
    return mm_block_resize2( mb, new_size, 0 );
}

size_t mm_block_size(mm_block_data_type *mb)
{
    return mb->used_;
}

size_t mm_block_capacity(struct mm_block_data *mb)
{
    return mb->capacity_;
}

size_t mm_block_available(struct mm_block_data *mb)
{
    return (mb->capacity_ - mb->used_);
}

void*  mm_block_data(struct mm_block_data *mb)
{
    return mb->data_;
}


int mm_block_clear(struct mm_block_data *mb)
{
    mb->used_ = 0;
    return 1;
}

int mm_block_swap(struct mm_block_data *lmb, struct mm_block_data *rmb)
{
    char   *tmp_data = lmb->data_;
    size_t  tmp_used = lmb->used_;
    size_t  tmp_capa = lmb->capacity_;

    lmb->data_      = rmb->data_ ;
    lmb->used_      = rmb->used_ ;
    lmb->capacity_  = rmb->capacity_;

    rmb->data_      = tmp_data;
    rmb->used_      = tmp_used;
    rmb->capacity_  = tmp_capa;

    return 1;
}

int mm_block_concat(struct mm_block_data *lmb, const void *data, size_t len)
{
    size_t old_used = lmb->used_;
    if( 0 == mm_block_resize2(lmb, old_used + len, 0) ) return 0;
    memcpy( &lmb->data_[old_used], data, len );
    return 1;
}

int mm_block_concat2(struct mm_block_data *lmb, const struct mm_block_data *rmb)
{
    return mm_block_concat(lmb, rmb->data_, rmb->used_);
}

int mm_block_push_back(struct mm_block_data *mb, char c)
{
    size_t old_capa = mb->capacity_;
    if( ( mb->used_ ) >= old_capa ) {
        size_t new_capa = mm_block_def_inc( old_capa );
        if( 0 == mm_block_reserve( mb, new_capa ) )
            return 0;
    }
    mb->data_[mb->used_++] = c;
    return 1;
}

void *mm_block_create_back( struct mm_block_data *mb, size_t count )
{
    size_t old_size = mb->used_;
    void *tail = NULL;
    int res = mm_block_resize( mb, old_size + count );
    if( 0 != res ) {
        tail = ( mb->data_ + old_size );
    }
    return tail;
}


