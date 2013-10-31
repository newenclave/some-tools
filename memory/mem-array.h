#if !defined(MEMARRAY_20132910_H)
#define MEMARRAY_20132910_H

#include <stdlib.h>

struct mem_array_data;

struct mem_array_data *mem_array_new( size_t element_size );
struct mem_array_data *mem_array_new2( size_t count, size_t element_size );
void                   mem_array_free( struct mem_array_data *mar );

void  *mem_array_at( struct mem_array_data *mar, size_t element_index );
size_t mem_array_size( struct mem_array_data *mar );
int    mem_array_push_back( struct mem_array_data *mar, void *element );
int    mem_array_push_back2( struct mem_array_data *mar, void *element, size_t count );
int    mem_array_resize( struct mem_array_data *mar, size_t new_count );
int    mem_array_reserve( struct mem_array_data *mar, size_t count );
size_t mem_array_available (struct mem_array_data *mar);

#define mem_array_create( type ) \
       mem_array_new( sizeof( type ) )

#define mem_array_create2( count, type ) \
       mem_array_new2( count, sizeof( type ) )

#endif // MEMARRAY_H
