#if !defined(MM_ARRAY_20132910_H)
#define MM_ARRAY_20132910_H

#include <stdlib.h>

typedef void  (* mm_array_element_free)( void *element );
typedef void *(* mm_array_element_copy)( void *new_place,
                                         void *element,
                                         size_t element_size );

typedef int   (* mm_array_compare)( const void *lelem, const void *relem );

struct mm_array;

struct mm_array *mm_array_new( size_t element_size );
struct mm_array *mm_array_new2( size_t count, size_t element_size );
struct mm_array *mm_array_new3( size_t count, size_t element_size,
                                     mm_array_element_free free_call);

void   mm_array_swap( struct mm_array *mar, struct mm_array *other );
void   mm_array_free( struct mm_array *mar );
void   mm_array_free2( struct mm_array *mar, mm_array_element_free free_call );
void   mm_array_set_free( struct mm_array *mar,
                          mm_array_element_free free_call );

void  *mm_array_begin( struct mm_array *mar );
void  *mm_array_end( struct mm_array *mar );
void  *mm_array_at( struct mm_array *mar, size_t element_index );

size_t mm_array_size( struct mm_array *mar );
size_t mm_array_element_size( struct mm_array *mar );


void  *mm_array_create_back( struct mm_array *mar, size_t count );
int    mm_array_push_back  ( struct mm_array *mar, void *element );
int    mm_array_push_back2 ( struct mm_array *mar, void *element,
                             size_t count );
int    mm_array_push_back3 ( struct mm_array *mar,
                             void *element, size_t count,
                             mm_array_element_copy copy_call);


void  *mm_array_create_front( struct mm_array *mar, size_t count );
int    mm_array_push_front  ( struct mm_array *mar, void *element );
int    mm_array_push_front2 ( struct mm_array *mar, void *element,
                              size_t count );
int    mm_array_push_front3 ( struct mm_array *mar,
                              void *element, size_t count,
                              mm_array_element_copy copy_call);

void  *mm_array_create_insertion( struct mm_array *mar,
                                  size_t pos, size_t count );
int    mm_array_insert ( struct mm_array *mar, void *element, size_t pos );
int    mm_array_insert2( struct mm_array *mar, void *element,
                            size_t pos, size_t count );
int    mm_array_insert3( struct mm_array *mar,
                         void *element, size_t pos, size_t count,
                         mm_array_element_copy copy_call);

int    mm_array_resize ( struct mm_array *mar, size_t new_count );
int    mm_array_resize2( struct mm_array *mar, size_t new_count,
                         mm_array_element_free free_call);

int    mm_array_reserve   ( struct mm_array *mar, size_t count );
size_t mm_array_available ( struct mm_array *mar);

int    mm_array_bin_search( struct mm_array *mar, void *element,
                            mm_array_compare cmp_call);

int    mm_array_bin_insert( struct mm_array *mar, void *element,
                            mm_array_compare cmp_call);
int    mm_array_bin_insert2( struct mm_array *mar, void *element,
                             mm_array_compare cmp_call,
                             mm_array_element_copy copy_call);

void  *mm_array_erase( struct mm_array *mar, size_t position, size_t count );
void  *mm_array_erase2( struct mm_array *mar, size_t position, size_t count,
                         mm_array_element_free free_call);

void   mm_array_reduce( struct mm_array *mar, size_t count );
void   mm_array_reduce2( struct mm_array *mar, size_t count,
                         mm_array_element_free free_call);

void   mm_array_reduce_from ( struct mm_array *mar, size_t position );
void   mm_array_reduce_from2( struct mm_array *mar, size_t position,
                              mm_array_element_free free_call);


#define MM_ARRAY_IS_END( mar, ptr ) ((ptr) == mm_array_end(mar))
#define MM_ARRAY_IS_BEGIN( mar, ptr ) ((ptr) == mm_array_begin(mar))

#define MM_ARRAY_CREATE( type_name ) mm_array_new( sizeof(type_name) )

#define MM_ARRAY_CREATE2( count, type_name )            \
            mm_array_new2(count, sizeof(type_name))

#define MM_ARRAY_CREATE3( count, type_name, free_call ) \
            mm_array_new3(count, sizeof(type_name), free_call)

#define MM_ARRAY_CAST_PTR_BEGIN( type_name, mar )       \
        ((type_name *)mm_array_begin( mar ))

#define MM_ARRAY_CAST_BEGIN( type_name, mar )           \
        (*MM_ARRAY_CAST_PTR_BEGIN(type_name, mar))

#define MM_ARRAY_CAST_PTR_END( type_name, mar )         \
        ((type_name *)mm_array_end( mar ))

#define MM_ARRAY_CAST_END( type_name, mar )             \
        (*MM_ARRAY_CAST_PTR_END( type_name, mar ))

#define MM_ARRAY_CAST_PTR_AT( type_name, mar, index )   \
        ((type_name *)mm_array_at( mar, (index) ))

#define MM_ARRAY_CAST_AT( type_name, mar, index )       \
        (*MM_ARRAY_CAST_PTR_AT( type_name, mar, index ))

#endif // MM_ARRAY_20132910_H
