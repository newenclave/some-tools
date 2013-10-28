
#include <stdio.h>

#include "bitpack.h"
#include "mem-block.h"

int main( )
{

    struct bit_pack_data *bpd = bp_new_bitpack_data( );

    printf( "is en: %d\n", bp_is_space_enough( bpd, 8 * 7 ) );


    int i = 0;
    for( i=0; i<10000; ++i )
        bp_add_bits( bpd, 11, 8 );

    //bp_dump( bpd );

    bp_delete_bitpack_data( bpd );

    return 0;
}
