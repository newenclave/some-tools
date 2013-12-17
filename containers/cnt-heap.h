#ifndef CNTHEAP_20131217_H
#define CNTHEAP_20131217_H

#include <stdlib.h>

struct cnt_heap;

typedef int  (* cnt_heap_data_compare)( const void *l, const void *r );
typedef int  (* cnt_heap_data_free) ( const void *l, const void *r );
typedef int  (* cnt_heap_data_copy) ( void *src, const void *new_value );

struct cnt_heap *cnt_heap_new  ( size_t element_size );

void cnt_heap_free  ( );
void cnt_heap_free2 ( cnt_heap_data_free free_call );

#endif // CNTHEAP_20131217_H
