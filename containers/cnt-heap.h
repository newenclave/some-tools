#ifndef CNT_HEAP_20131217_H
#define CNT_HEAP_20131217_H

#include <stdlib.h>

struct cnt_heap;

typedef int   (* cnt_heap_element_compare )( const void *l, const void *r,
                                                        size_t element_size );
typedef void  (* cnt_heap_element_free ) ( void *element );
typedef void *(* cnt_heap_element_copy)  ( void *dst, const void *src,
                                                          size_t element_size );

struct cnt_heap *cnt_heap_new  ( size_t element_size,
                                 cnt_heap_element_compare compare );
struct cnt_heap *cnt_heap_new2 ( size_t element_size,
                                 cnt_heap_element_compare compare,
                                 cnt_heap_element_free free_call );

void cnt_heap_set_free ( struct cnt_heap *heap,
                                    cnt_heap_element_free free_call );
void cnt_heap_set_copy ( struct cnt_heap *heap,
                                    cnt_heap_element_copy copy_call );

int cnt_heap_push  ( struct cnt_heap *heap, const void *element );
int cnt_heap_push2 ( struct cnt_heap *heap, const void *element,
                     cnt_heap_element_copy copy_call);

void cnt_heap_pop  ( struct cnt_heap *heap );
void cnt_heap_pop2 ( struct cnt_heap *heap, cnt_heap_element_free free_call );


void *cnt_heap_front( struct cnt_heap *heap );
const void *cnt_heap_const_front( const struct cnt_heap *heap );

void *cnt_heap_at( struct cnt_heap *heap, size_t index );
const void *cnt_heap_const_at( const struct cnt_heap *heap, size_t index );

size_t cnt_heap_size ( const struct cnt_heap *heap );

void cnt_heap_free  ( struct cnt_heap *heap );
void cnt_heap_free2 ( struct cnt_heap *heap, cnt_heap_element_free free_call );

#endif // CNTHEAP_20131217_H
