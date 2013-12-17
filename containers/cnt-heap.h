#ifndef CNTHEAP_20131217_H
#define CNTHEAP_20131217_H

#include <stdlib.h>

struct cnt_heap;

typedef int   (* cnt_heap_element_compare )( const void *l, const void *r,
                                                        size_t element_size );
typedef void  (* cnt_heap_element_free ) ( void *element );
typedef void *(* cnt_heap_element_copy)  ( void *dst, const void *src,
                                                          size_t element_size );

struct cnt_heap *cnt_heap_new  ( size_t element_size );
struct cnt_heap *cnt_heap_new2 ( size_t element_size,
                                 cnt_heap_element_compare compare );
struct cnt_heap *cnt_heap_new3 ( size_t element_size,
                                 cnt_heap_element_compare compare,
                                 cnt_heap_element_free free_call );

void cnt_heap_set_free ( struct cnt_heap *heap,
                                    cnt_heap_element_free free_call );
void cnt_heap_set_copy ( struct cnt_heap *heap,
                                    cnt_heap_element_copy copy_call );

int cnt_heap_insert ( struct cnt_heap *heap, const void *element );

size_t cnt_heap_size ( const struct cnt_heap *heap );

void cnt_heap_free  ( struct cnt_heap *heap );
void cnt_heap_free2 ( struct cnt_heap *heap, cnt_heap_element_free free_call );

#endif // CNTHEAP_20131217_H
