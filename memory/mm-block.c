
#include <memory.h>
#include <stdio.h>

#include "mm-block.h"

struct mm_block {
    char    *data_;
    size_t   capacity_;
    size_t   used_;
#ifdef MM_BLOCK_USE_CUSTOM_ALLOC
    mm_general_alloc alloc_;
    mm_general_free  free_;
#endif
};

typedef struct mm_block mm_block_data_type;

static const size_t void_ptr_size      =   sizeof(void *);
static const size_t void_ptr_size_mask =   sizeof(void *) - 1;

#define mm_block_def_inc(size) (size + (size >> 1))

#define mm_block_available_local( mb ) ((mb)->capacity_ - (mb)->used_)

#define mm_block_fix_size( new_size )                               \
      ((new_size + void_ptr_size_mask) & (~(void_ptr_size_mask)))

#define mm_block_fix_size0( new_size )                              \
      ( new_size <= void_ptr_size)                                  \
      ? void_ptr_size                                               \
      : ((new_size + void_ptr_size_mask) & (~(void_ptr_size_mask)))


size_t mm_block_calc_prefer_size( size_t old_capa, size_t desired_size )
{
    size_t new_capa = mm_block_fix_size(mm_block_def_inc(old_capa));
    desired_size = mm_block_fix_size(desired_size);

    if( new_capa > desired_size ) desired_size = new_capa;

    return desired_size;
}

struct mm_block *mm_block_new_reserved( size_t reserve_size )
{
    mm_block_data_type *new_block =
            (mm_block_data_type *)malloc(sizeof(mm_block_data_type));

    size_t new_size = mm_block_fix_size0(reserve_size);

    new_block->data_ = NULL;
    if( new_size ) {
        new_block->data_ = (char *)malloc(new_size);
        if( NULL == new_block->data_ ) {
            free(new_block);
            return NULL;
        }
    }

    new_block->capacity_ = new_size;
    new_block->used_     = 0;

    return new_block;
}

/// mm_block_new
/// creating new memory block
mm_block_data_type *mm_block_new( size_t init_size )
{
    mm_block_data_type *new_block = mm_block_new_reserved( init_size );

    if( new_block ) {
        new_block->used_ = init_size;
        if( init_size )
            memset(new_block->data_, 0, init_size);
    }

    return new_block;
}

struct mm_block *mm_block_new_copy( const struct mm_block *oth )
{
    size_t new_size = oth->used_;
    struct mm_block *new_block = mm_block_new(new_size);
    if( new_block && (new_size > 0)) {
        memcpy( new_block->data_, oth->data_, new_size );
    }
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

int mm_block_reserve(struct mm_block *mb, size_t new_size)
{
    static int i=0;

    if( new_size <= mb->capacity_ ) return 1;

    new_size = mm_block_calc_prefer_size( mb->capacity_, new_size );

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

int mm_block_resize2(struct mm_block *mb, size_t new_size, int c)
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

int mm_block_resize(struct mm_block *mb, size_t new_size)
{
    return mm_block_resize2( mb, new_size, 0 );
}

size_t mm_block_size(mm_block_data_type *mb)
{
    return mb->used_;
}

size_t mm_block_capacity(struct mm_block *mb)
{
    return mb->capacity_;
}

size_t mm_block_available(struct mm_block *mb)
{
    return mm_block_available_local( mb );
}

void*  mm_block_data(struct mm_block *mb)
{
    return mb->data_;
}

int mm_block_clear(struct mm_block *mb)
{
    mb->used_ = 0;
    return 1;
}

int mm_block_swap(struct mm_block *lmb, struct mm_block *rmb)
{
    char   *tmp_data = lmb->data_;
    size_t  tmp_used = lmb->used_;
    size_t  tmp_capa = lmb->capacity_;

    lmb->data_       = rmb->data_ ;
    lmb->used_       = rmb->used_ ;
    lmb->capacity_   = rmb->capacity_;

    rmb->data_       = tmp_data;
    rmb->used_       = tmp_used;
    rmb->capacity_   = tmp_capa;

    return 1;
}

int mm_block_concat(struct mm_block *lmb, const void *data, size_t len)
{
    size_t old_used = lmb->used_;
    if( 0 == mm_block_resize2(lmb, old_used + len, 0) ) return 0;
    memcpy( &lmb->data_[old_used], data, len );
    return 1;
}

int mm_block_concat2(struct mm_block *lmb, const struct mm_block *rmb)
{
    return mm_block_concat(lmb, rmb->data_, rmb->used_);
}

int mm_block_push_back(struct mm_block *mb, char c)
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

void *mm_block_create_back( struct mm_block *mb, size_t count )
{
    size_t old_size = mb->used_;
    void *tail = NULL;
    int res = mm_block_resize( mb, old_size + count );
    if( 0 != res ) {
        tail = ( mb->data_ + old_size );
    }
    return tail;
}

void *mm_block_create_insertion( struct mm_block *mb,
                                 size_t position, size_t count )
{
    void *block = NULL;

    if( position == mb->used_ ) {
        block = mm_block_create_back( mb, count );
    } else if( mm_block_available_local( mb ) < count ) {

        size_t new_size
                = mm_block_calc_prefer_size( mb->capacity_, mb->used_ + count );

        struct mm_block *new_block = mm_block_new_reserved( new_size );

        if( new_block ) {
            new_block->used_ = mb->used_+count;
            size_t new_tail_shift  = position + count;
            if( position ) {
                memcpy( new_block->data_, mb->data_, position );
            }
            memcpy( new_block->data_ + new_tail_shift,
                           mb->data_ + position,
                           mb->used_ - position);
            mm_block_swap( mb, new_block );
            mm_block_free( new_block );
            block = mb->data_ + position;
        }
    } else {
        char *from = mb->data_ + position;
        memmove( from + count, from, mb->used_ - position);
        mb->used_ += count;
        block = from;
    }

    return block;
}

void *mm_block_create_front( struct mm_block *mb, size_t count )
{
    return mm_block_create_insertion( mb, 0, count );
}
