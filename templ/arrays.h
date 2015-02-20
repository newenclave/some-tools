#ifndef ARRAYS_H
#define ARRAYS_H

#include <stdlib.h>
#include <memory.h>


#define array_define_custom_type( type, type_name )             \
  typedef struct type_name {                                    \
      type    *dat_;                                            \
      size_t   len_;                                            \
      size_t   cap_;                                            \
  } type_name

#define array_define_type( type )            \
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
            do {                                                               \
                array_insert_block(arr, pos, 1, result );                      \
                if( result ) {                                                 \
                    array_at(arr, pos) = value;                                \
                }                                                              \
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

#define array_reserve( arr, size, result )                                     \
    if( (size) > array_capacity( arr ) ) {                                     \
        size_t diff__ = (size) - array_capacity( arr );                        \
        array_extend_capacity( arr, diff__, result );                          \
    }

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
            int cmp__ = (compare)( array_at( arr, middle__ ), (value) );       \
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
            int cmp__ = (compare)( array_at( arr, middle__ ), (value) );       \
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
            int cmp__ = (compare)( array_at( arr, middle__ ), (value) );       \
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
