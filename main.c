
#include <stdio.h>

#include "bitpack.h"

int main( )
{
    struct bit_pack_data *bpd = bp_new_bitpack_data( );

    printf( "is en: %d\n", bp_is_space_enough( bpd, 8 * 7 ) );

    bp_add_bits( bpd, 10, 4 );
    bp_add_bits( bpd, 10, 4 );
    bp_add_bits( bpd, 0xFFFFFFFF, 32 );
    bp_add_bits( bpd, 3, 3 );

    printf( "current size: %d (%d)\n",
                bp_get_size( bpd ), bp_get_padd( bpd ) );
    printf( "is en: %d\n", bp_is_space_enough( bpd, 61 ) );

    bp_dump( bpd );

    bp_delete_bitpack_data( bpd );

    return 0;
}
