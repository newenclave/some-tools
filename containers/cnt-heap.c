
#include <string.h>

#include "cnt-heap.h"
#include "../memory/mm-array.h"

struct cnt_heap
{
    struct mm_array *container_;
    cnt_heap_element_compare compare_;
    cnt_heap_element_copy    copy_;
};

static void *cnt_heap_alloc( size_t size )
{
    return malloc( size );
}

static void cnt_heap_memfree( void *ptr )
{
    return free( ptr );
}

static void *cnt_heap_default_copy( void *dst, const void *src,
                                                size_t element_size )
{
    return memcpy( dst, src, element_size );
}

static int cnt_heap_default_comparator( const void *l, const void *r,
                                                           size_t element_size )
{
    return memcmp( l, r, element_size );
}

struct cnt_heap *cnt_heap_new3 ( size_t element_size,
                                 cnt_heap_element_compare compare,
                                 cnt_heap_element_free free_call )
{
    struct cnt_heap *new_heap =
            (struct cnt_heap *)cnt_heap_alloc(sizeof(*new_heap));
    if( new_heap ) {
        new_heap->container_ = mm_array_new3( 0, element_size, free_call );
        if( new_heap->container_ ) {
            new_heap->compare_ = compare;
        } else {
            cnt_heap_memfree(new_heap);
            new_heap = 0;
        }
    }
    return new_heap;
}


struct cnt_heap *cnt_heap_new2 ( size_t element_size,
                                 cnt_heap_element_compare compare )
{
    return cnt_heap_new3(element_size, compare, NULL);
}


struct cnt_heap *cnt_heap_new ( size_t element_size )
{
    return cnt_heap_new3(element_size,
                         cnt_heap_default_comparator, NULL);
}

void cnt_heap_set_free ( struct cnt_heap *heap,
                                    cnt_heap_element_free free_call )
{
    mm_array_set_free( heap->container_, free_call );
}

void cnt_heap_free2 ( struct cnt_heap *heap, cnt_heap_element_free free_call )
{
    mm_array_free2( heap->container_, (mm_array_element_free)(free_call) );
    cnt_heap_memfree( heap );
}

void cnt_heap_free ( struct cnt_heap *heap )
{
    mm_array_free( heap->container_ );
    cnt_heap_memfree( heap );
}
