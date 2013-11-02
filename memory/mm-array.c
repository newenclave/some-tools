
#include "mm-array.h"
#include "mm-block.h"

struct mm_array_data {
    struct mm_block  *mmblock_;
    mm_array_element_free  free_;
    size_t                 element_size_;
};

#define mm_array_at_local( arr, index ) \
    (((char *)mm_block_data((arr)->mmblock_)) + ((index)*(arr)->element_size_))

#define mm_elements_size( arr, count ) \
        ((arr)->element_size_ * count)

#define mm_element_shift( ptr, element_size, count ) \
    (((char *)ptr) + ((element_size) * (count)))

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
    return (mm_block_size( mar->mmblock_ ) / mar->element_size_);
}

void *mm_array_create_back( struct mm_array_data *mar, size_t count )
{
    void *tail = mm_block_create_back( mar->mmblock_,
                                       mm_elements_size(mar, count));
    return tail;
}

void mm_array_copy_elements( void *dst, void *src,
                             size_t element_size, size_t count,
                             mm_array_element_copy copy_call)
{
    if( dst && copy_call ) {
        while ( count-- ) {
            copy_call( dst, src, element_size );
            dst = mm_element_shift( dst, element_size, 1 );
            src = mm_element_shift( src, element_size, 1 );
        }
    }
}

int mm_array_push_back2( struct mm_array_data *mar, void *element, size_t count)
{
    int res = mm_block_concat(mar->mmblock_, element,
                              mm_elements_size(mar, count));
    return res;
}

int mm_array_push_back( struct mm_array_data *mar, void *element )
{
    return mm_array_push_back2( mar, element, 1 );
}

int mm_array_push_back3( struct mm_array_data *mar,
                         void *element, size_t count,
                         mm_array_element_copy copy_call)
{
    void *tail = mm_block_create_back( mar->mmblock_,
                                       mm_elements_size(mar, count));
    mm_array_copy_elements(tail, element, mar->element_size_, count, copy_call);
    return (tail != NULL);
}

void *mm_array_create_front( struct mm_array_data *mar, size_t count )
{
    void *head = mm_block_create_front(mar->mmblock_,
                                       mm_elements_size(mar, count));
    return head;
}

int mm_array_push_front2 ( struct mm_array_data *mar, void *element,
                           size_t count )
{
    void *head = mm_array_create_front( mar, count );
    if( head ) {
        memcpy( head, element, mm_elements_size( mar, count) );
    }
    return (head != NULL);
}

int mm_array_push_front  ( struct mm_array_data *mar, void *element )
{
    return mm_array_push_front2( mar, element, 1 );
}

int mm_array_push_front3 ( struct mm_array_data *mar,
                              void *element, size_t count,
                              mm_array_element_copy copy_call)
{
    void *head = mm_array_create_front( mar, count );
    mm_array_copy_elements(head, element, mar->element_size_, count, copy_call);
    return (head != NULL);
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
    res = mm_block_resize2( mar->mmblock_, mm_elements_size(mar, new_count), 0);
    return res;
}

int mm_array_resize( struct mm_array_data *mar, size_t new_count )
{
    return mm_array_resize2( mar, new_count, mar->free_ );
}

int mm_array_reserve( struct mm_array_data *mar, size_t count )
{
    return mm_block_reserve( mar->mmblock_, mm_elements_size(mar, count) );
}

size_t mm_array_available (struct mm_array_data *mar)
{
    return (mm_block_available( mar->mmblock_ ) / mar->element_size_);
}
