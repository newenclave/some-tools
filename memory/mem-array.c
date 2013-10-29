
#include "mem-array.h"
#include "mem-block.h"

struct mem_array_data {
    struct mem_block_data *mem_;
    size_t                 element_size_;
};


struct mem_array_data *mem_array_new2( size_t count, size_t element_size )
{
    struct mem_array_data *new_data =
            (mem_array_data *)malloc( sizeof( struct mem_array_data ) );
    if( NULL == new_data )
        return NULL;
    new_data->mem_ = mem_block_new( count * element_size );

    if( NULL == new_data->mem_ ) {
        free( new_data );
        return NULL;
    }
    new_data->element_size_ = element_size;
}

struct mem_array_data *mem_array_new( size_t element_size )
{

}


int mem_block_free(struct mem_block_data *mb);
