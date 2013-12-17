
#include "cnt-heap.h"
#include "../memory/mm-array.h"

struct cnt_heap
{
    struct mm_array *container_;
    cnt_heap_data_compare compare_;
    cnt_heap_data_copy    copy_;
    cnt_heap_data_free    free_;
};

static void *cnt_heap_alloc( size_t size )
{
    return malloc( size );
}

static void cnt_heap_memfree( void *ptr )
{
    return free( ptr );
}

struct cnt_heap *cnt_heap_new ( size_t element_size )
{
    struct cnt_heap *new_heap =
            (struct cnt_heap *)cnt_heap_alloc(sizeof(*new_heap));
    if( new_heap ) {
        new_heap->container_ = mm_array_new( element_size );
        if( new_heap->container_ ) {
            ;;;
        } else {
            cnt_heap_memfree(new_heap);
            new_heap = 0;
        }
    }
    return new_heap;
}



