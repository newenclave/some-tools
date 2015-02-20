
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


int main( )
{

    string_array sa = array_init;

    array_push_back( sa, "1234567890" );
    array_push_back( sa, "1234567891" );
    array_push_back( sa, "1234567892" );
    array_push_back( sa, "1234567893" );

    const size_t array_size = 20;

    int_array_type ai = array_init;

    int i;

    size_t data = 17;

    for( i=0; i<array_size; i++ ) {
        array_push_back(ai, i);
    }

    array_bin_search_compare( sa, "1234567892", strcmp, data );

    printf( "Index is %lu is end: %d\n", data, data == array_lenght(ai) );

    show_array( &ai );

    array_free(sa);
    array_free(ai);

    return 0;
}
