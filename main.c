
#include <stdio.h>
#include <stdint.h>

#include "templ/vector.h"
#include "templ/lists.h"
#include "templ/deque.h"

#include "lists/bilinked-list.h"
#include "inc/struct-fields.h"

vector_define_type(int);
vector_define_custom_type( const char *, string_array );

deque_define_custom( int, deq, 64, malloc, free );

deque_define_custom( float, float_deque_type, 64, malloc, free );

struct test {
    int  i_;
    char put_;
};

vector_define_custom_type(struct test, test_vector);

void show_array( const int_vector_type *arr )
{
    size_t i;
    vector_foreach( *arr, i ) {
        printf( "%d ", vector_at( *arr, i ) );
    }
    printf( "\n" );
}

void show_array2( const string_array *arr )
{
    size_t i;
    vector_foreach( *arr, i ) {
        printf( "%s ", vector_at( *arr, i ) );
    }
    printf( "\n" );
}

#define MAXCOUNT 1000000

int main( )
{
    float_deque_type fd;
    float_deque_type_init( &fd );
    float_deque_type_push_back( &fd, 10 );

    for( int i=0; i<MAXCOUNT; i++ ) {
        float_deque_type_push_back( &fd, (float)(i/10.0) );
    }

    for( int i=0; i<MAXCOUNT; i++ ) {
        float_deque_type_emplace_front( &fd );
    }

    for( int i=0; i<MAXCOUNT; i++ ) {
        float_deque_type_push_front( &fd, (float)(i/10.0) );
        float_deque_type_pop_back( &fd );
    }

    while( deque_lenght( fd ) ) {
        printf( "%f ", *float_deque_type_front( &fd ) );
        float_deque_type_pop_front( &fd );
    }

    float_deque_type_free( &fd );

    return 0;
    string_array   sa = vector_init;

    test_vector     ta = vector_init;

    test_vector_push_back( &ta, (struct test){ i_:1, put_: '1' } );
    test_vector_push_back( &ta, (struct test){ i_:2, put_: '2' } );
    test_vector_push_back( &ta, (struct test){ i_:3, put_: '3' } );
    test_vector_push_back( &ta, (struct test){ i_:4, put_: '4' } );

    unsigned int j;
    vector_foreach( ta, j ) {
        printf( "%d -> %c\n", vector_at(ta, j).i_, vector_at(ta, j).put_ );
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

    vector_push_back( sa, "01" );
    vector_push_back( sa, "02" );
    vector_push_back( sa, "03" );
    vector_push_back( sa, "04" );
    vector_push_back( sa, "04" );
    vector_push_back( sa, "04" );
    vector_push_back( sa, "04" );
    vector_push_back( sa, "04" );
    vector_push_back( sa, "05" );
    vector_push_back( sa, "06" );

    int i;

    size_t position;

//    array_bin_upper_bound_compare( sa, "04", strcmp, position );
    vector_insert( sa, position, "!!", i );

    printf( "Index is %lu is end: %d\n",
            position, (position == vector_lenght(sa)) );

    show_array2( &sa );

    vector_free(sa);
    vector_free(ta);

    return 0;
}
