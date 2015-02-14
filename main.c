
#include <stdio.h>
#include "templ/arrays.h"
#include "templ/lists.h"

list_define_type(int);
array_define_type(int);

#define array_copy( dst, src )                  \
    array_resize( dst, array_lenght(src) );     \
    memcpy( &array_at(dst, 0), &array_at(src, 0), array_full_size( arr ) )

int main( )
{
    int_list_type il = list_init(0);
    int_array_type arr = array_init;
    int_array_type arr2 = array_init;

    int_list_type * t = &il;
    int i;


    for(i=0; i<100; i++) {
        list_insert_value( *t, i, t );
        array_push_back( arr, i );
    }

    array_remove( arr, 0, 90 );
    array_copy( arr2, arr );

//    list_foreach( il, t ) {
//        printf( "value: %d\n", t->dat_ );
//    }

    array_foreach( arr2, i ) {
        printf( "value: %d\n", array_at(arr2, i) );
    }

    list_free(il);
    array_free(arr);

    return 0;
}
