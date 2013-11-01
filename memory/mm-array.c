
#include "mm-array.h"
#include "mm-block.h"

struct mm_array_data {
    struct mm_block_data *mem_;
    size_t                 element_size_;
};

struct mm_array_data *mm_array_new2( size_t count, size_t element_size )
{
    struct mm_array_data *new_data =
        (struct mm_array_data *)calloc( 1, sizeof( struct mm_array_data ) );

    if( new_data ) {
        new_data->mem_ = mm_block_new( count * element_size );
        if( NULL == new_data->mem_ ) {
            free( new_data );
            return NULL;
        }
        new_data->element_size_ = element_size;
    }
    return new_data;
}

struct mm_array_data *mm_array_new( size_t element_size )
{
    return mm_array_new2( 0, element_size );
}

void mm_array_free( struct mm_array_data *mar )
{
    if( NULL != mar ) {
        mm_block_free( mar->mem_ );
        free( mar );
    }
}

void  *mm_array_at( struct mm_array_data *mar, size_t element_index )
{
    char *d = mm_block_data( mar->mem_ );
    return (d + (element_index * mar->element_size_));
}

size_t mm_array_size( struct mm_array_data *mar )
{
    return (mm_block_size( mar->mem_ ) / mar->element_size_);
}

int mm_array_push_back2( struct mm_array_data *mar, void *element, size_t count )
{
    return mm_block_concat( mar->mem_, element, count * mar->element_size_ );
}

int mm_array_push_back( struct mm_array_data *mar, void *element )
{
    return mm_array_push_back2( mar, element, 1 );
}

int mm_array_resize( struct mm_array_data *mar, size_t new_count )
{
    return mm_block_resize2( mar->mem_, new_count * mar->element_size_, 0 );
}

int mm_array_reserve( struct mm_array_data *mar, size_t count )
{
    return mm_block_reserve( mar->mem_, count * mar->element_size_ );
}

size_t mm_array_available (struct mm_array_data *mar)
{
    return (mm_block_available( mar->mem_ ) / mar->element_size_);
}
