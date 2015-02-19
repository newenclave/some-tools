
#include <stdio.h>
#include <stdint.h>

#include "templ/arrays.h"
#include "templ/lists.h"

array_define_type(int);

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

    const size_t array_size = 100;

    int_array_type ai = array_init;

    int i;

    size_t parent = array_size;
    size_t next   = parent >> 1;

    const int data = 17;

    for( i=0; i<array_size; i++ ) {
        array_push_back(ai, i);
    }

    show_array( &ai );

    return 0;
}
