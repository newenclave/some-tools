
#include <stdio.h>
#include "templ/arrays.h"
#include "templ/lists.h"

list_define_type(int);

int main( )
{
    int_list_type il = list_init(0);
    int_list_type * t = &il;
    int i;

    for(i=1; i<100; i++) {
        list_insert_value( *t, i, t );
    }

    list_foreach( il, t ) {
        printf( "value: %d\n", t->dat_ );
    }

    list_free(il);

    return 0;
}
