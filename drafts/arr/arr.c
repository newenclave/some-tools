#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>

#define array_define_type( type )            \
  typedef struct type##_array_type {         \
      type    *dat_;                         \
      size_t   len_;                         \
      size_t   cap_;                         \
  } type##_array_type

#define array_default_increase(arr) \
                    ((arr).cap_ ? (((arr).cap_ >> 1) + ((arr).cap_)) : 4)

#define array_elements_size( arr, count ) (sizeof(*(arr).dat_) * count)

#define array_push_back( arr, value )                                          \
    if( array_lenght(arr) < array_capacity(arr) ) {                            \
        array_at(arr, (arr).len_++) = (value);                                 \
    } else {                                                                   \
        int res = 0;                                                           \
        array_extend_capacity( arr, array_default_increase(arr), res );        \
        if( res != 0 ) {                                                       \
            array_at(arr, array_lenght(arr)++) = (value);                      \
        }                                                                      \
    }

#define array_push_front( arr, value )                                         \
    if( array_lenght(arr) < array_capacity(arr) ) {                            \
        memmove( &array_at(arr, 1), &array_at(arr, 0),                         \
                 array_elements_size(arr, array_lenght(arr)) );                \
        array_at(arr, 0) = (value);                                            \
        array_lenght(arr)++;                                                   \
    } else {                                                                   \
        int res = 0;                                                           \
        array_extend_capacity( arr, array_default_increase(arr), res );        \
        if( res != 0 ) {                                                       \
            memmove( &array_at(arr, 1), &array_at(arr, 0),                     \
                     array_elements_size(arr, array_lenght(arr)) );            \
            array_at(arr, 0) = (value);                                        \
            array_lenght(arr)++;                                               \
        }                                                                      \
    }

#define array_resize( arr, new_size, result )                                  \
    if( new_size <= array_capacity( arr ) ) {                                  \
        arr.len_ = new_size;                                                   \
    } else {                                                                   \
        array_extend_capacity( arr, new_size, result );                        \
        if( result ) {                                                         \
            (arr).len_ = (arr).cap_ = new_size;                                \
        }                                                                      \
    }

#define array_reduce( arr, count ) (arr).len_--

#define array_reserve( arr, size, result )                                     \
    if( (size) > array_capacity( arr ) ) {                                     \
        size_t diff = array_capacity( arr ) - (size);                          \
        array_extend_capacity( arr, diff, result );                            \
    }

#define array_extend_capacity( arr, size, result )                             \
    do {                                                                       \
        void *tmp;                                                             \
        if((arr).dat_) {                                                       \
            tmp = realloc( (arr).dat_,                                         \
                              array_elements_size(arr, (arr).cap_)             \
                            + array_elements_size(arr, size) );                \
        } else {                                                               \
            tmp = malloc( array_elements_size(arr, size) );                    \
        }                                                                      \
        if( tmp != NULL ) {                                                    \
            (arr).dat_  = tmp;                                                 \
            (arr).cap_ += (size);                                              \
        }                                                                      \
        result = (tmp != NULL);                                                \
    } while(0)

#define array_resize_nocheck(arr, new_size)  \
        array_resize(arr, (new_size), (int *)(NULL))

#define array_foreach( arr, i ) for( i=0; i<(arr.len_); ++i )

#define array_foreach_value( arr, i, v )                                       \
    for( i=0, v=(arr).dat_[i]; i<(arr.len_); ++i, v=(arr).dat_[i] )

#define array_lenght( arr )             (arr).len_
#define array_capacity( arr )           (arr).cap_
#define array_init                      { NULL, 0, 0 }
#define array_at( arr, index )          (arr).dat_[index]
#define array_valid( arr )              ((arr).dat_ != NULL)
#define array_valid_index( arr, index ) ((arr).len_ < (index))

#define array_free( arr )      free((arr).dat_), (arr).len_ = (arr).cap_ = 0

typedef struct {
    int i;
    int j;
} test_type;

array_define_type(int);
array_define_type(double);
array_define_type(char);
array_define_type(test_type);

int main(void)
{
    int_array_type          arr  = array_init;
    double_array_type       arr2 = array_init;
    char_array_type         arr3 = array_init;
    test_type_array_type    arr4 = array_init;

    //int err;

    size_t i;
    for( i=0; i<10; ++i ) {
        array_push_back( arr, i + 1 );
        array_push_front( arr2, (double)(i)/3 );
        array_push_back( arr3, (char)(i & 0xFF ) );
        test_type t = { i, i };
        array_push_front( arr4, t );
    }

    array_foreach( arr, i ) {
        //printf( "%lu => %d\n", i, array_at(arr, i) );
    }

    array_foreach( arr2, i ) {
        printf( "%lu => %f\n", i, array_at(arr2, i) );
    }

    array_foreach( arr3, i ) {
        //printf( "%c", array_at(arr3, i) );
    }

    array_foreach( arr4, i ) {
        printf( "%d, %d\n", array_at(arr4, i).i, array_at(arr4, i).j );
    }

    array_free( arr);
    array_free(arr2);
    array_free(arr3);
    array_free(arr4);

    return 0;
}

