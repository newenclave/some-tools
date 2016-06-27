
#include <stdio.h>
#include <stdint.h>

#include "templ/arrays.h"
#include "templ/lists.h"
#include "templ/deque.h"

#include "lists/bilinked-list.h"
#include "inc/struct-fields.h"

array_define_type(int);
array_define_custom_type( const char *, string_array );

deque_define_custom( int, deq, 64, malloc, free );

deque_define_type( float );

struct test {
    int  i_;
    char put_;
};

array_define_custom_type(struct test, test_array);

void show_array( const int_array_type *arr )
{
    size_t i;
    array_foreach( *arr, i ) {
        printf( "%d ", array_at( *arr, i ) );
    }
    printf( "\n" );
}

void show_array2( const string_array *arr )
{
    size_t i;
    array_foreach( *arr, i ) {
        printf( "%s ", array_at( *arr, i ) );
    }
    printf( "\n" );
}


int main( )
{
    float_deque_type fd;
    float_deque_type_init( &fd );
    float_deque_type_push_back( &fd, 10 );

    for( int i=0; i<10; i++ ) {
        float_deque_type_push_back( &fd, (float)(i/10.0) );
    }

    while( deque_lenght( fd ) ) {
        printf( "%f ", *float_deque_type_front( &fd ) );
        float_deque_type_pop_front( &fd );
    }

    float_deque_type_free( &fd );

    return 0;
    string_array   sa = array_init;

    test_array     ta = array_init;

    test_array_push_back( &ta, (struct test){ i_:1, put_: '1' } );
    test_array_push_back( &ta, (struct test){ i_:2, put_: '2' } );
    test_array_push_back( &ta, (struct test){ i_:3, put_: '3' } );
    test_array_push_back( &ta, (struct test){ i_:4, put_: '4' } );

    unsigned int j;
    array_foreach( ta, j ) {
        printf( "%d -> %c\n", array_at(ta, j).i_, array_at(ta, j).put_ );
    }

    string_array_push_back( &sa, "01" );
    string_array_push_back( &sa, "02" );
    string_array_push_back( &sa, "03" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "05" );
    string_array_push_back( &sa, "06" );

    array_push_back( sa, "01" );
    array_push_back( sa, "02" );
    array_push_back( sa, "03" );
    array_push_back( sa, "04" );
    array_push_back( sa, "04" );
    array_push_back( sa, "04" );
    array_push_back( sa, "04" );
    array_push_back( sa, "04" );
    array_push_back( sa, "05" );
    array_push_back( sa, "06" );

    int i;

    size_t position;

    array_bin_upper_bound_compare( sa, "04", strcmp, position );
    array_insert( sa, position, "!!", i );

    printf( "Index is %lu is end: %d\n",
            position, (position == array_lenght(sa)) );

    show_array2( &sa );

    array_free(sa);
    array_free(ta);

    return 0;
}
