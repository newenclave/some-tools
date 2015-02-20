
#include <stdio.h>
#include <stdint.h>

#include "templ/arrays.h"
#include "templ/lists.h"

array_define_type(int);
array_define_custom_type(const char *, string_array);

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

    const size_t array_size = 20;

    int_array_type ai = array_init;
    string_array   sa = array_init;

    array_push_back( sa, "1" );
    array_push_back( sa, "2" );
    array_push_back( sa, "3" );
    array_push_back( sa, "4" );
    array_push_back( sa, "4" );
    array_push_back( sa, "4" );
    array_push_back( sa, "4" );
    array_push_back( sa, "4" );
    array_push_back( sa, "5" );
    array_push_back( sa, "6" );

    int i;

    size_t data = 17;

    for( i=0; i<array_size; i++ ) {
        array_push_back(ai, i);
    }

    array_bin_upper_bound_compare( sa, "4", strcmp, data );
    array_insert( sa, data, "!", i );

    printf( "Index is %lu is end: %d\n", data, (data == array_lenght(sa)) );

    show_array2( &sa );

    array_free(sa);
    array_free(ai);

    return 0;
}
