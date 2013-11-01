
#include "mm-array.h"
#include "mm-block.h"

struct mm_array_data {
    struct mm_block_data  *mmblock_;
    mm_array_element_free  free_;
    size_t                 element_size_;
    size_t                 current_count_;
};

#define mm_array_at_local( arr, index ) \
    (((char *)mm_block_data((arr)->mmblock_)) + ((index)*(arr)->element_size_))

struct mm_array_data *mm_array_new3( size_t count, size_t element_size,
                                     mm_array_element_free free_call)
{
    struct mm_array_data *new_data =
        (struct mm_array_data *)malloc(sizeof( struct mm_array_data ) );

    if( new_data ) {
        new_data->mmblock_ = mm_block_new( count * element_size );
        if( NULL == new_data->mmblock_ ) {
            free( new_data );
            return NULL;
        }
        new_data->element_size_ = element_size;
        new_data->free_         = free_call;
        new_data->current_count_= count;
    }
    return new_data;
}


struct mm_array_data *mm_array_new2( size_t count, size_t element_size )
{
    return mm_array_new3( count, element_size, NULL );
}

struct mm_array_data *mm_array_new( size_t element_size )
{
    return mm_array_new3( 0, element_size, NULL );
}

void mm_array_free2( struct mm_array_data *mar,
                     mm_array_element_free free_call )
{
    if( NULL != mar ) {
        if( free_call ) {
            size_t count = mm_array_size( mar );
            size_t i;
            for( i=0; i<count; ++i )
                free_call( mm_array_at_local( mar, i ) );
        }
        mm_block_free( mar->mmblock_ );
        free( mar );
    }
}

void mm_array_free( struct mm_array_data *mar )
{
    mm_array_free2(mar, mar->free_);
}

void mm_array_set_free( struct mm_array_data *mar,
                        mm_array_element_free free_call )
{
    mar->free_ = free_call;
}

void  *mm_array_at( struct mm_array_data *mar, size_t element_index )
{
    return mm_array_at_local( mar, element_index );
}

size_t mm_array_size( struct mm_array_data *mar )
{
    return mar->current_count_;
            //(mm_block_size( mar->mmblock_ ) / mar->element_size_);
}

int mm_array_push_back2( struct mm_array_data *mar, void *element, size_t count )
{
    int res = mm_block_concat(mar->mmblock_, element, count*mar->element_size_);
    if( res ) mar->current_count_ += count;
    return res;
}

int mm_array_push_back( struct mm_array_data *mar, void *element )
{
    return mm_array_push_back2( mar, element, 1 );
}

int mm_array_push_back3( struct mm_array_data *mar, void *element,
                         mm_array_element_copy copy_call)
{
    size_t count = mm_array_size( mar );
    size_t new_mem_size = (count + 1) * mar->element_size_;
    int resize_res = mm_block_resize( mar->mmblock_, new_mem_size );

    if( resize_res ) {
        if( copy_call )
            copy_call( mm_array_at( mar, count ), element, mar->element_size_ );
        ++mar->current_count_;
    }
    return resize_res;
}

int mm_array_resize2( struct mm_array_data *mar, size_t new_count,
                      mm_array_element_free free_call)
{
    size_t count = mm_array_size( mar );
    int res = 0;
    if( (new_count < count) && free_call ) {
        for( ; count > new_count; --count )
            free_call( mm_array_at_local( mar, count-1 ) );
    }
    res = mm_block_resize2( mar->mmblock_, new_count * mar->element_size_, 0 );
    if( res ) {
        mar->current_count_ = new_count;
    }
    return res;
}

int mm_array_resize( struct mm_array_data *mar, size_t new_count )
{
    return mm_array_resize2( mar, new_count, mar->free_ );
}

int mm_array_reserve( struct mm_array_data *mar, size_t count )
{
    return mm_block_reserve( mar->mmblock_, count * mar->element_size_ );
}

size_t mm_array_available (struct mm_array_data *mar)
{
    return (mm_block_available( mar->mmblock_ ) / mar->element_size_);
}
