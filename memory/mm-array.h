#if !defined(MM_ARRAY_20132910_H)
#define MM_ARRAY_20132910_H

#include <stdlib.h>

struct mm_array_data;

struct mm_array_data *mm_array_new( size_t element_size );
struct mm_array_data *mm_array_new2( size_t count, size_t element_size );
void                   mm_array_free( struct mm_array_data *mar );

void  *mm_array_at( struct mm_array_data *mar, size_t element_index );
size_t mm_array_size( struct mm_array_data *mar );

int    mm_array_push_back( struct mm_array_data *mar, void *element );
int    mm_array_push_back2( struct mm_array_data *mar, void *element, size_t count );

int    mm_array_resize( struct mm_array_data *mar, size_t new_count );

int    mm_array_reserve( struct mm_array_data *mar, size_t count );
size_t mm_array_available (struct mm_array_data *mar);

#define mm_array_create( type ) mm_array_new( sizeof( type ) )
#define mm_array_create2( count, type ) mm_array_new2(count, sizeof(type))

#endif // MM_ARRAY_20132910_H
