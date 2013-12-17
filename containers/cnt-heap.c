
#include <string.h>

#include "cnt-heap.h"
#include "../memory/mm-array.h"

struct cnt_heap
{
    struct mm_array          *container_;
    cnt_heap_element_compare  compare_;
    cnt_heap_element_copy     copy_;
    cnt_heap_element_free     free_;
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

struct cnt_heap *cnt_heap_new2 ( size_t element_size,
                                 cnt_heap_element_compare compare,
                                 cnt_heap_element_free free_call )
{
    struct cnt_heap *new_heap =
            (struct cnt_heap *)cnt_heap_alloc(sizeof(*new_heap));
    if( new_heap ) {
        new_heap->container_ = mm_array_new2( element_size, 1 );
        if( new_heap->container_ ) {
            new_heap->compare_ = compare;
            new_heap->copy_    = cnt_heap_default_copy;
            new_heap->free_    = free_call;
        } else {
            cnt_heap_memfree(new_heap);
            new_heap = 0;
        }
    }
    return new_heap;
}


struct cnt_heap *cnt_heap_new ( size_t element_size,
                                 cnt_heap_element_compare compare )
{
    return cnt_heap_new2(element_size, compare, NULL);
}

void cnt_heap_set_free ( struct cnt_heap *heap, cnt_heap_element_free free_call)
{
    heap->free_ = free_call;
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

//siftdown( array heap )
//{
//    heap_size = len(heap);
//    next = 1;
//    while( (next / 2) <= heap_size ) {
//        minimum = (next / 2) - 1;
//        if( minimum + 1 < heap_size ) {
//            minimum = heap[minimum] < heap[minimum+1]
//                      ? minimum : minimum + 1;
//        }
//        if( heap[minimum] < heap[next-1] )
//            swap( heap[next-1], heap[minimum] );
//        else
//            break;
//        next = minimum+1;
//    }
//}

static void sift_down( struct cnt_heap *heap, size_t heap_size )
{
    struct mm_array *arr = heap->container_;
    const size_t element_size = mm_array_element_size(arr);

    size_t next     = 1;
    size_t children = next << 1;

    void *tmp_store = mm_array_at( arr, heap_size );

    while( children <= heap_size ) {
        size_t minimum = children - 1;
        if( children < heap_size ) {
            const int cmp_children =
                    heap->compare_( mm_array_at( arr, minimum + 1 ),
                                    mm_array_at( arr, minimum ),
                                    element_size);
            if( cmp_children < 0) minimum++;
        }

        void *src_elem = mm_array_at( arr, next-1  );
        void *min_elem = mm_array_at( arr, minimum );

        const int cmp = heap->compare_( min_elem, src_elem, element_size);
        if( cmp < 0 ) {
            swap_elements( src_elem, min_elem, tmp_store,
                                        element_size, heap->copy_ );
            next = minimum + 1;
            children = next << 1;
        } else {
            children = heap_size + 1; // break;
        }
    }
}

void cnt_heap_pop2 ( struct cnt_heap *heap, cnt_heap_element_free free_call )
{
    const size_t element_size = mm_array_element_size( heap->container_ );
    const size_t arr_size = mm_array_size( heap->container_ ) - 1;
    void *first = mm_array_begin( heap->container_ );

    if( free_call )
        free_call( first );

    if( arr_size > 1 ) {
        heap->copy_( first,
                 mm_array_at(heap->container_, arr_size-1), element_size);

        sift_down( heap, arr_size - 1 );
    }

    mm_array_reduce( heap->container_, 1 );
}

void cnt_heap_pop ( struct cnt_heap *heap )
{
    cnt_heap_pop2( heap, heap->free_ );
}

//siftup( array heap )
//{
//    heap_size = len(heap);
//    while( heap_size * 2 ) {
//        parent = (heap_size * 2);
//        if( heap[heap_size - 1] < heap[parent - 1] ) {
//            swap( heap[heap_size - 1], heap[parent - 1] );
//            heap_size = parent;
//        } else {
//            break;
//        }
//    }
//}

static void sift_up( struct cnt_heap *heap, size_t heap_size )
{
    const size_t element_size = mm_array_element_size( heap->container_ );
    void *tmp_store = mm_array_at( heap->container_, heap_size );

    size_t parent_index = (heap_size >> 1);

    while( parent_index ) {

        void *current = mm_array_at( heap->container_, heap_size - 1 );
        void *parent  = mm_array_at( heap->container_, parent_index - 1 );

        const int cmp_res = heap->compare_( current, parent, element_size );

        if( cmp_res < 0 ) {
            swap_elements( current, parent, tmp_store,
                           element_size, heap->copy_ );
            heap_size = parent_index;
            parent_index >>= 1;
        } else {
            parent_index = 0; // break
        }
    }
}

int cnt_heap_push2 ( struct cnt_heap *heap, const void *element,
                     cnt_heap_element_copy copy_call)
{
    const size_t element_size = mm_array_element_size( heap->container_ );
    const size_t arr_size = mm_array_size( heap->container_ );
    int res = mm_array_extend( heap->container_, 1 );

    if( res ) {
        copy_call( mm_array_at( heap->container_, arr_size - 1 ),
                                                       element, element_size );
        sift_up( heap, arr_size );
    }
    return res;

}

int cnt_heap_push ( struct cnt_heap *heap, const void *element )
{
    return cnt_heap_push2( heap, element, heap->copy_ );
}

void *cnt_heap_front( struct cnt_heap *heap )
{
    return mm_array_begin( heap->container_ );
}

const void *cnt_heap_const_front( const struct cnt_heap *heap )
{
    return mm_array_const_begin( heap->container_ );
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
    cnt_heap_free2( heap, heap->free_ );
}

