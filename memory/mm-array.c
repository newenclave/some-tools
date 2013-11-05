
#include <string.h>
#include "mm-array.h"
#include "mm-block.h"

struct mm_array {
    struct mm_block  *mmblock_;
    mm_array_element_free  free_;
    size_t                 element_size_;
};

#define mm_array_at_local( arr, index )                 \
    (((char *)mm_block_data((arr)->mmblock_)) + ((index)*(arr)->element_size_))

#define mm_elements_size( arr, count )                  \
        ((arr)->element_size_ * count)

#define mm_element_shift( ptr, element_size, count )    \
    (((char *)ptr) + ((element_size) * (count)))

struct mm_array *mm_array_new3( size_t count, size_t element_size,
                                     mm_array_element_free free_call)
{
    struct mm_array *new_data =
        (struct mm_array *)malloc(sizeof( struct mm_array ) );

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


struct mm_array *mm_array_new2( size_t count, size_t element_size )
{
    return mm_array_new3( count, element_size, NULL );
}

struct mm_array *mm_array_new( size_t element_size )
{
    return mm_array_new3( 0, element_size, NULL );
}

void mm_array_free2( struct mm_array *mar,
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

void mm_array_free( struct mm_array *mar )
{
    mm_array_free2(mar, mar->free_);
}

void mm_array_set_free( struct mm_array *mar,
                        mm_array_element_free free_call )
{
    mar->free_ = free_call;
}

void  *mm_array_at( struct mm_array *mar, size_t element_index )
{
    return mm_array_at_local( mar, element_index );
}

size_t mm_array_size( struct mm_array *mar )
{
    return (mm_block_size( mar->mmblock_ ) / mar->element_size_);
}

void *mm_array_begin( struct mm_array *mar )
{
    return mm_array_at_local( mar, 0);
}

void  *mm_array_end( struct mm_array *mar )
{
    return mm_array_at_local( mar, mm_array_size(mar));
}

size_t mm_array_element_size( struct mm_array *mar )
{
    return mar->element_size_;
}

void *mm_array_create_back( struct mm_array *mar, size_t count )
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

int mm_array_push_back2( struct mm_array *mar, void *element, size_t count)
{
    int res = mm_block_concat(mar->mmblock_, element,
                              mm_elements_size(mar, count));
    return res;
}

int mm_array_push_back( struct mm_array *mar, void *element )
{
    return mm_array_push_back2( mar, element, 1 );
}

int mm_array_push_back3( struct mm_array *mar,
                         void *element, size_t count,
                         mm_array_element_copy copy_call)
{
    void *tail = mm_block_create_back( mar->mmblock_,
                                       mm_elements_size(mar, count));
    mm_array_copy_elements(tail, element, mar->element_size_, count, copy_call);
    return (tail != NULL);
}

void *mm_array_create_front( struct mm_array *mar, size_t count )
{
    void *head = mm_block_create_front(mar->mmblock_,
                                       mm_elements_size(mar, count));
    return head;
}

int mm_array_push_front2 ( struct mm_array *mar, void *element,
                           size_t count )
{
    void *head = mm_array_create_front( mar, count );
    if( head ) {
        memcpy( head, element, mm_elements_size( mar, count) );
    }
    return (head != NULL);
}

int mm_array_push_front  ( struct mm_array *mar, void *element )
{
    return mm_array_push_front2( mar, element, 1 );
}

int mm_array_push_front3 ( struct mm_array *mar,
                              void *element, size_t count,
                              mm_array_element_copy copy_call)
{
    void *head = mm_array_create_front( mar, count );
    mm_array_copy_elements(head, element, mar->element_size_, count, copy_call);
    return (head != NULL);
}

int mm_array_resize2( struct mm_array *mar, size_t new_count,
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

int mm_array_resize( struct mm_array *mar, size_t new_count )
{
    return mm_array_resize2( mar, new_count, mar->free_ );
}

int mm_array_reserve( struct mm_array *mar, size_t count )
{
    return mm_block_reserve( mar->mmblock_, mm_elements_size(mar, count) );
}

size_t mm_array_available (struct mm_array *mar)
{
    return (mm_block_available( mar->mmblock_ ) / mar->element_size_);
}

void  *mm_array_create_insertion( struct mm_array *mar,
                                  size_t pos, size_t count )
{
    void *insertion =
        mm_block_create_insertion( mar->mmblock_,
               mm_elements_size( mar, pos ),
               mm_elements_size( mar, count ));
    return insertion;
}

int mm_array_insert2 ( struct mm_array *mar, void *element,
                              size_t pos, size_t count )
{
    void * insertion = mm_array_create_insertion( mar, pos, count );
    if( insertion ) {
        memcpy( insertion, element, mm_elements_size(mar, count));
    }
    return (insertion != NULL);
}

int mm_array_insert  ( struct mm_array *mar,
                                  void *element, size_t pos )
{
    (void)(pos);
    return mm_array_insert( mar, element, 1 );
}

int mm_array_insert3 ( struct mm_array *mar,
                       void *element, size_t pos, size_t count,
                       mm_array_element_copy copy_call)
{
    void * insertion = mm_array_create_insertion( mar, pos, count );
    mm_array_copy_elements( insertion, element,
                                mar->element_size_, count, copy_call );
    return (insertion != NULL);
}

int mm_array_bin_lower_bound( struct mm_array *mar,
                              void    *element, mm_array_compare cmp_call,
                              size_t  *position )
{
    size_t right  =  mm_array_element_size( mar );
    size_t left   =  0;
    size_t middle =  0;
    int cmp       = -1;
    while( (right != left) && (cmp != 0) ) {
        middle = left + ((right - left) >> 1);
        void *data = mm_array_at( mar, middle );
        cmp    = cmp_call( element, data );
        if( cmp != 0 ) {
            if( cmp < 0 ) {
                right = middle;
            } else if( left == middle ) {
                middle = left + 1;
                right  = left;
            } else {
                left  = middle;
            }
        }
    }
    *position = middle;
    return (cmp == 0);
}

int mm_array_bin_search( struct mm_array *mar, void *element,
                         mm_array_compare cmp_call )
{
    size_t pos = 0;
    int res = mm_array_bin_lower_bound( mar, element, cmp_call, &pos );
    return res;
}

int mm_array_bin_insert2( struct mm_array *mar, void *element,
                             mm_array_compare cmp_call,
                             mm_array_element_copy copy_call)
{
    size_t pos = 0;
    mm_array_bin_lower_bound( mar, element, cmp_call, &pos );
    return mm_array_insert3(mar, element, pos, 1, copy_call);
}


int mm_array_bin_insert( struct mm_array *mar, void *element,
                         mm_array_compare cmp_call)
{
    return mm_array_bin_insert2( mar, element, cmp_call, NULL );
}
