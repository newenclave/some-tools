#if !defined(MM_ARRAY_20132910_H)
#define MM_ARRAY_20132910_H

#include <stdlib.h>

typedef void (* mm_array_element_free)( void *element );
typedef void (* mm_array_element_copy)( void *new_place,
                                        void *element,
                                        size_t element_size );

struct mm_array_data;

struct mm_array_data *mm_array_new( size_t element_size );
struct mm_array_data *mm_array_new2( size_t count, size_t element_size );
struct mm_array_data *mm_array_new3( size_t count, size_t element_size,
                                     mm_array_element_free free_call);

void                  mm_array_free( struct mm_array_data *mar );
void                  mm_array_free2( struct mm_array_data *mar,
                                      mm_array_element_free free_call );

void                  mm_array_set_free( struct mm_array_data *mar,
                                         mm_array_element_free free_call );


void  *mm_array_at( struct mm_array_data *mar, size_t element_index );
size_t mm_array_size( struct mm_array_data *mar );

int    mm_array_push_back( struct mm_array_data *mar, void *element );
int    mm_array_push_back2( struct mm_array_data *mar, void *element,
                            size_t count );
int    mm_array_push_back3( struct mm_array_data *mar, void *element,
                            mm_array_element_copy copy_call);

void  *mm_array_create_back( struct mm_array_data *mar, size_t count );

int    mm_array_resize( struct mm_array_data *mar, size_t new_count );
int    mm_array_resize2( struct mm_array_data *mar, size_t new_count,
                         mm_array_element_free free_call);

int    mm_array_reserve( struct mm_array_data *mar, size_t count );
size_t mm_array_available (struct mm_array_data *mar);

#define mm_array_create( type ) mm_array_new( sizeof( type ) )
#define mm_array_create2( count, type ) mm_array_new2(count, sizeof(type))
#define mm_array_create3( count, type, free_call ) \
    mm_array_new3(count, sizeof(type), free_call)

#endif // MM_ARRAY_20132910_H
