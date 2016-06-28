#ifndef TEMPL_ARRAYS_H
#define TEMPL_ARRAYS_H

#include <stdlib.h>
#include <memory.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
  #define INLINE_KEYWORD
#else
  #define INLINE_KEYWORD inline
#endif


#define array_define_custom( type, type_name,                                  \
                             allo_f, reallo_f, deallo_f, move_f )              \
    typedef struct _##type_name {                                                \
        type    *dat_;                                                         \
        size_t   len_;                                                         \
        size_t   cap_;                                                         \
    } type_name;                                                               \
\
static INLINE_KEYWORD \
int type_name##_extend_capacity( type_name *arr, size_t size )                 \
{                                                                              \
    void *tmp;                                                                 \
    if( arr->dat_ ) {                                                          \
        tmp = (reallo_f)( arr->dat_,                                           \
                          array_elements_size( *arr, arr->cap_ + size ) );     \
    } else {                                                                   \
        tmp = (allo_f)( array_elements_size( *arr, size ) );                   \
    }                                                                          \
    if( tmp != NULL ) {                                                        \
        arr->dat_  = tmp;                                                      \
        arr->cap_ += size;                                                     \
    }                                                                          \
    return tmp != NULL;                                                        \
}                                                                              \
\
static INLINE_KEYWORD \
int type_name##_resize( type_name *arr, size_t size )                          \
{                                                                              \
    int res = 1;                                                               \
    if( size > arr->cap_ ) {                                                   \
        res = type_name##_extend_capacity( arr, size - arr->cap_ );            \
    }                                                                          \
    if( res ) {                                                                \
        arr->len_ = size;                                                      \
    }                                                                          \
    return res;                                                                \
}                                                                              \
\
static INLINE_KEYWORD \
int type_name##_reserve( type_name *arr, size_t size )                         \
{                                                                              \
    int res = 1;                                                               \
    if( size > arr->cap_ ) {                                                   \
        res = type_name##_extend_capacity( arr, size - arr->cap_ );            \
    }                                                                          \
    return res;                                                                \
}                                                                              \
\
static INLINE_KEYWORD \
int type_name##_extend( type_name *arr, size_t size )                          \
{                                                                              \
    return type_name##_resize( arr, arr->len_ + size );                        \
}                                                                              \
\
static INLINE_KEYWORD \
int type_name##_reduce( type_name *arr, size_t size )                          \
{                                                                              \
    arr->len_ -= size;                                                         \
    return 1;                                                                  \
}                                                                              \
\
static INLINE_KEYWORD \
int type_name##_push_back( type_name *arr, type value )                        \
{                                                                              \
    int res = 1;                                                               \
    if( arr->len_ == arr->cap_ ) {                                             \
        res = type_name##_extend_capacity( arr, array_default_increase(*arr) );\
    }                                                                          \
    if( res ) {                                                                \
        arr->dat_[arr->len_++] = value;                                        \
    }                                                                          \
    return res;                                                                \
}                                                                              \
\
static INLINE_KEYWORD \
type *type_name##_emplace_back( type_name *arr )                        \
{                                                                              \
    int res = 1;                                                               \
    if( arr->len_ == arr->cap_ ) {                                             \
        res = type_name##_extend_capacity( arr, array_default_increase(*arr) );\
    }                                                                          \
    if( res ) {                                                                \
        return &arr->dat_[arr->len_++];                                        \
    }                                                                          \
    return NULL;                                                               \
}                                                                              \
\
static INLINE_KEYWORD \
int type_name##_insert_block( type_name *arr, size_t pos, size_t count )       \
{                                                                              \
    int res = 1;                                                               \
    if( (arr->len_ + count) > arr->cap_ )  {                                   \
        const size_t def_resize = array_default_increase(*arr);                \
        res = type_name##_extend_capacity( arr, count > def_resize             \
                                                        ? count                \
                                                        : def_resize );        \
    }                                                                          \
    if(res ) {                                                                 \
        move_f( &arr->dat_[pos + count],                                       \
                &arr->dat_[pos], array_elements_size( *arr, arr->len_-pos) );  \
        arr->len_ += count;                                                    \
    }                                                                          \
    return res;                                                                \
}                                                                              \
\
static INLINE_KEYWORD \
int type_name##_insert( type_name *arr, size_t pos, type value )               \
{                                                                              \
    if( type_name##_insert_block( arr, pos, 1 ) ) {                            \
        arr->dat_[pos] = value;                                                \
        return 1;                                                              \
    }                                                                          \
    return 0;                                                                  \
}                                                                              \
\
static INLINE_KEYWORD \
type *type_name##_emplace_at( type_name *arr, size_t pos )                     \
{                                                                              \
    if( type_name##_insert_block( arr, pos, 1 ) ) {                            \
        return &arr->dat_[pos];                                                \
    }                                                                          \
    return NULL;                                                               \
}                                                                              \
\
static INLINE_KEYWORD \
int type_name##_push_front( type_name *arr, type value )                       \
{                                                                              \
    return type_name##_insert( arr, 0, value );                                \
}                                                                              \
\
static INLINE_KEYWORD \
void type_name##_free( type_name *arr )                                        \
{                                                                              \
    (deallo_f)( arr->dat_ );                                                   \
    arr->len_ = arr->cap_ = 0;                                                 \
}\
typedef void *(* type_name##_allocator  )( size_t );                           \
typedef void *(* type_name##_reallocator)( void *, size_t );                   \
typedef void  (* type_name##_deallocator)( void * );                           \
typedef int   (* type_name##_compare    )( const type *, const type * )

#define array_define_custom_type( type, type_name )     \
    array_define_custom( type, type_name,               \
                         malloc, realloc, free, memmove )


#define array_define_type( type )                   \
    array_define_custom_type( type, type##_array_type )

#define array_default_increase(arr) \
                    ((arr).cap_ ? (((arr).cap_ >> 1) + ((arr).cap_)) : 4)

#define array_elements_size( arr, count ) (sizeof(*(arr).dat_) * (count))
#define array_full_size( arr ) array_elements_size( arr, array_lenght(arr) )

#define array_push_back( arr, value )                                          \
    if( array_lenght(arr) < array_capacity(arr) ) {                            \
        array_at(arr, (arr).len_++) = (value);                                 \
    } else {                                                                   \
        int res__ = 0;                                                         \
        array_extend_capacity_check( arr, array_default_increase(arr), res__ );\
        if( res__ != 0 ) {                                                     \
            array_at(arr, array_lenght(arr)++) = (value);                      \
        }                                                                      \
    }

#define array_push_front( arr, value )                                         \
    do {                                                                       \
        int res__ = 0;                                                         \
        array_insert_block( arr, 0, 1, res );                                  \
        if( res__ ) {                                                          \
            array_at(arr, 0) = (value);                                        \
        }                                                                      \
    } while( 0 )

#define array_insert_block( arr, pos, count, result )                          \
    if( array_lenght(arr) + (count) < array_capacity(arr) )  {                 \
        memmove( &array_at(arr, (pos) + (count)),                              \
                 &array_at(arr, pos ),                                         \
                  array_elements_size(arr, array_lenght(arr) - (pos)) );       \
        array_lenght(arr) += (count);                                          \
        result = 1;                                                            \
    } else {                                                                   \
        size_t def_resize = array_default_increase(arr);                       \
        array_extend_capacity_check( arr, (count) > def_resize                 \
                                                  ? (count)                    \
                                                  : def_resize, result );      \
        if( result != 0 ) {                                                    \
            memmove( &array_at(arr, (pos) + (count)),                          \
                     &array_at(arr, (pos) ),                                   \
                      array_elements_size(arr, array_lenght(arr) - (pos)) );   \
            array_lenght(arr) += count;                                        \
        }                                                                      \
    }

#define array_insert( arr, pos, value, result )                                \
    do {                                                                       \
        array_insert_block(arr, pos, 1, result );                              \
        if( result ) {                                                         \
            array_at(arr, pos) = value;                                        \
        }                                                                      \
    } while(0)


#define array_resize_check( arr, new_size, result )                            \
    do {                                                                       \
        if( new_size <= array_capacity( arr ) ) {                              \
            (arr).len_ = new_size;                                             \
            result = 1;                                                        \
        } else {                                                               \
            array_extend_capacity_check( arr, new_size, result );              \
            if( result ) {                                                     \
                (arr).len_ = (arr).cap_ = new_size;                            \
            }                                                                  \
        }                                                                      \
    } while(0)

#define array_resize( arr, new_size )                                          \
    do {                                                                       \
        if( (new_size) > array_capacity( arr ) ) {                             \
            array_extend_capacity( arr, new_size - array_capacity( arr ) );    \
        }                                                                      \
        (arr).len_ = (new_size);                                               \
    } while(0)


#define array_reduce( arr, count ) (arr).len_ -= (count)

#define array_remove( arr, pos, count )         \
    memmove( &array_at(arr, pos),               \
             &array_at(arr, (pos) + (count)),   \
              array_elements_size( (arr),       \
                     array_lenght(arr) -        \
                     (count) - (pos) ) ),       \
    (arr).len_ -= count

#define array_extend_capacity_check( arr, size, result )                       \
    do {                                                                       \
        void *tmp__;                                                           \
        if((arr).dat_) {                                                       \
            tmp__ = realloc( (arr).dat_,                                       \
                              array_elements_size(arr, (arr).cap_)             \
                            + array_elements_size(arr, size) );                \
        } else {                                                               \
            tmp__ = malloc( array_elements_size(arr, size) );                  \
        }                                                                      \
        if( tmp__ != NULL ) {                                                  \
            (arr).dat_  = tmp__;                                               \
            (arr).cap_ += (size);                                              \
        }                                                                      \
        result = (tmp__ != NULL);                                              \
    } while(0)

#define array_extend_capacity( arr, size )                                     \
    do {                                                                       \
        void *tmp__;                                                           \
        if((arr).dat_) {                                                       \
            tmp__ = realloc( (arr).dat_,                                       \
                              array_elements_size(arr, (arr).cap_)             \
                            + array_elements_size(arr, size) );                \
        } else {                                                               \
            tmp__ = malloc( array_elements_size(arr, size) );                  \
        }                                                                      \
        if( tmp__ != NULL ) {                                                  \
            (arr).dat_  = tmp__;                                               \
            (arr).cap_ += (size);                                              \
        }                                                                      \
    } while(0)

#define array_reserve( arr, size, result )                                     \
    if( (size) > array_capacity( arr ) ) {                                     \
        size_t diff__ = (size) - array_capacity( arr );                        \
        array_extend_capacity( arr, diff__ );                                  \
    }


/// Warning! side effects!
#define array_bin_search( arr, value, result )                                 \
    do {                                                                       \
        size_t result__ = array_lenght( arr );                                 \
        size_t parent__ = result__;                                            \
        size_t next__   = 0;                                                   \
        while( next__ < parent__ ) {                                           \
            size_t middle__ = next__ + ( ( parent__ - next__ ) >> 1 );         \
            if( array_at( arr, middle__ ) < (value) ) {                        \
                next__ = middle__ + 1;                                         \
            } else if( (value) < array_at( arr, middle__ ) ) {                 \
                parent__ = middle__;                                           \
            } else {                                                           \
                result__ = middle__;                                           \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        result = result__;                                                     \
    } while(0)

/// Warning! side effects!
/// With comparator!
///  -1; 0; 1;
#define array_bin_search_compare( arr, value, compare, result )                \
    do {                                                                       \
        size_t result__ = array_lenght( arr );                                 \
        size_t parent__ = result__;                                            \
        size_t next__   = 0;                                                   \
        while( next__ < parent__ ) {                                           \
            size_t middle__ = next__ + ( ( parent__ - next__ ) >> 1 );         \
            int cmp__ = (compare)( &array_at( arr, middle__ ), (value) );      \
            if( cmp__ < 0 ) {                                                  \
                next__ = middle__ + 1;                                         \
            } else if( 0 < cmp__ ) {                                           \
                parent__ = middle__;                                           \
            } else {                                                           \
                result__ = middle__;                                           \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        result = result__;                                                     \
    } while(0)

/// Warning! side effects!
#define array_bin_lower_bound( arr, value, result )                            \
    do {                                                                       \
        size_t parent__ = array_lenght( arr );                                 \
        size_t next__   = 0;                                                   \
        while( next__ < parent__ ) {                                           \
            size_t middle__ = next__ + ( ( parent__ - next__ ) >> 1 );         \
            if( array_at( arr, middle__ ) < (value) ) {                        \
                next__ = middle__ + 1;                                         \
            } else {                                                           \
                parent__ = middle__;                                           \
            }                                                                  \
        }                                                                      \
        result = next__;                                                       \
    } while(0)

/// Warning! side effects!
#define array_bin_upper_bound( arr, value, result )                            \
    do {                                                                       \
        size_t parent__ = array_lenght( arr );                                 \
        size_t next__   = 0;                                                   \
        while( next__ < parent__ ) {                                           \
            size_t middle__ = next__ + ( ( parent__ - next__ ) >> 1 );         \
            if( (value) < array_at( arr, middle__ ) ) {                        \
                parent__ = middle__;                                           \
            } else {                                                           \
                next__ = middle__ + 1;                                         \
            }                                                                  \
        }                                                                      \
        result = next__;                                                       \
    } while(0)

/// Warning! side effects!
#define array_bin_lower_bound_compare( arr, value, compare, result )           \
    do {                                                                       \
        size_t parent__ = array_lenght( arr );                                 \
        size_t next__   = 0;                                                   \
        while( next__ < parent__ ) {                                           \
            size_t middle__ = next__ + ( ( parent__ - next__ ) >> 1 );         \
            int cmp__ = (compare)( &array_at( arr, middle__ ), (value) );      \
            if( cmp__ < 0 ) {                                                  \
                next__ = middle__ + 1;                                         \
            } else {                                                           \
                parent__ = middle__;                                           \
            }                                                                  \
        }                                                                      \
        result = next__;                                                       \
    } while(0)

/// Warning! side effects!
#define array_bin_upper_bound_compare( arr, value, compare, result )           \
    do {                                                                       \
        size_t parent__ = array_lenght( arr );                                 \
        size_t next__   = 0;                                                   \
        while( next__ < parent__ ) {                                           \
            size_t middle__ = next__ + ( ( parent__ - next__ ) >> 1 );         \
            int cmp__ = (compare)( &array_at( arr, middle__ ), (value) );      \
            if( 0 < cmp__ ) {                                                  \
                parent__ = middle__;                                           \
            } else {                                                           \
                next__ = middle__ + 1;                                         \
            }                                                                  \
        }                                                                      \
        result = next__;                                                       \
    } while(0)


#define array_foreach( arr, i ) for( i=0; i<((arr).len_); i++ )

/*
#define array_foreach_value( arr, i, v )                                       \
    for( i=0, v=(arr).dat_[i]; i<((arr).len_); i++, v=(arr).dat_[i] )
*/

#define array_copy( dst, src ) \
    array_copy_slice( dst, src, 0, array_lenght(src) )

#define array_copy_slice( dst, src, pos, count )      \
    array_resize( dst, count );                       \
    memcpy( &array_at(dst, 0),                        \
            &array_at(src, pos),                      \
             array_elements_size( arr, count ) )

#define array_lenght( arr )             (arr).len_
#define array_capacity( arr )           (arr).cap_
#define array_init                      { NULL, 0, 0 }
#define array_place_init(arr)           (arr).dat_ = NULL, \
                                        (arr).len_ = (arr).cap_ = 0
#define array_at( arr, index )          (arr).dat_[index]
#define array_valid( arr )              ((arr).dat_ != NULL)
#define array_valid_index( arr, index ) ((arr).len_ < (index))

#define array_free( arr )      free((arr).dat_), (arr).len_ = (arr).cap_ = 0

#endif // ARRAYS_H
