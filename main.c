
#include <stdio.h>
#include <stdint.h>

#include "templ/arrays.h"
#include "templ/lists.h"

array_define_type(int)
array_define_custom_type(const char *, string_array)

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
    string_array   sa = array_init;

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

//    array_push_back( sa, "01" );
//    array_push_back( sa, "02" );
//    array_push_back( sa, "03" );
//    array_push_back( sa, "04" );
//    array_push_back( sa, "04" );
//    array_push_back( sa, "04" );
//    array_push_back( sa, "04" );
//    array_push_back( sa, "04" );
//    array_push_back( sa, "05" );
//    array_push_back( sa, "06" );

    int i;

    size_t position;

    array_bin_upper_bound_compare( sa, "04", strcmp, position );
    array_insert( sa, position, "!!", i );

    printf( "Index is %lu is end: %d\n",
            position, (position == array_lenght(sa)) );

    show_array2( &sa );

    array_free(sa);

    return 0;
}
