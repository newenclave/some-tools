
#include <stdio.h>
#include "templ/arrays.h"

typedef struct {
    int x, y;
    double disposition;
} test_type;

array_define_type(double);
array_define_type(test_type);

int main( )
{
    double_array_type da = array_init;
    test_type_array_type tat = array_init;

    size_t i;
    for( i=0; i<10; ++i) {
        array_push_back( da, (double)(i) / 17 );
        test_type t = { i, i + 1, (double)(i) / 17 };
        array_push_back( tat, t );
    }

    for( i=0; i<10; ++i) {
        array_push_front( da, (double)(i) / 17 );
    }

    array_foreach( da, i ) {
        printf( "%lu:%lf\n", i, array_at(da, i));

    }

    array_foreach( tat, i ) {
        printf( "%d:%d = %lf\n",
                 array_at(tat, i).x,
                 array_at(tat, i).y,
                 array_at(tat, i).disposition );
    }

    array_free( da );
    return 0;
}
