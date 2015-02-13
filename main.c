
#include <stdio.h>
#include "templ/arrays.h"

array_define_type(double);

int main( )
{
    double_array_type da = array_init;

    size_t i;
    for( i=0; i<10; ++i) {
        array_push_back( da, (double)(i) / 17 );
    }

    array_foreach( da, i ) {
        printf("%lu => %lf\n", i, array_at(da, i));
    }

    return 0;
}
