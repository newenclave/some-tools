
#include "mem-array.h"
#include "mem-block.h"

struct mem_array_data {
    struct mem_block_data *mem_;
    size_t                 element_size_;
};

struct mem_array_data *mem_array_new2( size_t count, size_t element_size )
{
    struct mem_array_data *new_data =
            (struct mem_array_data *)malloc( sizeof( struct mem_array_data ) );
    if( NULL == new_data )
        return NULL;
    new_data->mem_ = mem_block_new( count * element_size );

    if( NULL == new_data->mem_ ) {
        free( new_data );
        return NULL;
    }
    new_data->element_size_ = element_size;
    return new_data;
}

struct mem_array_data *mem_array_new( size_t element_size )
{
    return mem_array_new2( 0, element_size );
}

int mem_array_free( struct mem_array_data *mar )
{
    if( NULL != mar ) {
        mem_block_free( mar->mem_ );
        free( mar );
    }
    return 1;
}

void  *mem_array_at( struct mem_array_data *mar, size_t element_index )
{
    char *d = mem_block_data( mar->mem_ );
    return (d + (element_index * mar->element_size_));
}

size_t mem_array_size( struct mem_array_data *mar )
{
    return (mem_block_size( mar->mem_ ) / mar->element_size_);
}

int mem_array_push_back2( struct mem_array_data *mar, void *element, size_t count )
{
    return mem_block_concat( mar->mem_, element, count * mar->element_size_ );
}

int mem_array_push_back( struct mem_array_data *mar, void *element )
{
    return mem_array_push_back2( mar, element, 1 );
}

int mem_array_resize( struct mem_array_data *mar, size_t new_count )
{
    return mem_block_resize2( mar->mem_, new_count * mar->element_size_, 0 );
}

int mem_array_reserve( struct mem_array_data *mar, size_t count )
{
    return mem_block_reserve( mar->mem_, count * mar->element_size_ );
}

size_t mem_array_available (struct mem_array_data *mar)
{
    return (mem_block_available( mar->mem_ ) / mar->element_size_);
}
