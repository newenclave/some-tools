
#include <string.h>

#include "cnt-heap.h"
#include "../memory/mm-array.h"

struct cnt_heap
{
    struct mm_array          *container_;
    cnt_heap_element_compare  compare_;
    cnt_heap_element_copy     copy_;
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
        new_heap->container_ = mm_array_new3( element_size, 1, free_call );
        if( new_heap->container_ ) {
            new_heap->compare_ = compare;
            new_heap->copy_    = cnt_heap_default_copy;
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

void cnt_heap_set_free ( struct cnt_heap *heap, cnt_heap_element_free free_call)
{
    mm_array_set_free( heap->container_, free_call );
}

void cnt_heap_set_copy ( struct cnt_heap *heap,
                                    cnt_heap_element_copy copy_call )
{
    heap->copy_ = copy_call;
}

size_t cnt_heap_size ( const struct cnt_heap *heap )
{
    return (mm_array_size( heap->container_ ) - 1);
}

// =============

static void swap_elements( void *l, void *r, void *tmp, size_t size,
                           cnt_heap_element_copy copy_call )
{
    copy_call( tmp, l, size );
    copy_call( l,   r, size );
    copy_call( r, tmp, size );
}

static void sift_down( struct cnt_heap *heap, size_t heap_size )
{

}

static void sift_up( struct cnt_heap *heap, size_t heap_size )
{
    const size_t element_size = mm_array_element_size( heap->container_ );
    void *tmp_store = mm_array_at( heap->container_, heap_size );

    size_t parent_index = (heap_size >> 1);
    while( parent_index ) {
        void *current = mm_array_at( heap->container_, heap_size - 1 );
        void *parent  = mm_array_at( heap->container_, parent_index - 1 );
        const int cmp_res   = heap->compare_( current, parent, element_size );
        if( cmp_res < 0 ) {
            swap_elements( current, parent, tmp_store,
                           element_size, heap->copy_ );
            parent_index >>= 1;
        } else {
            parent_index = 0; // break
        }
    }
}

int cnt_heap_insert ( struct cnt_heap *heap, const void *element )
{
    const size_t element_size = mm_array_element_size( heap->container_ );
    const size_t arr_size = mm_array_size( heap->container_ );
    int res = mm_array_resize( heap->container_,  arr_size + 1 );
    if( res ) {
        heap->copy_( mm_array_at( heap->container_, arr_size - 1 ),
                                                       element, element_size );
        sift_up( heap, arr_size );
    }
    return res;
}

// =============

void cnt_heap_free2 ( struct cnt_heap *heap, cnt_heap_element_free free_call )
{
    mm_array_reduce2(  heap->container_, 1, NULL );
    mm_array_free2( heap->container_, free_call );
    cnt_heap_memfree( heap );
}

void cnt_heap_free ( struct cnt_heap *heap )
{
    mm_array_reduce2(  heap->container_, 1, NULL );
    mm_array_free( heap->container_ );
    cnt_heap_memfree( heap );
}
