
#include <stdio.h>

#include "bitpack.h"
#include "mem-block.h"

int main( )
{

    struct mem_block_data *nd = mem_block_new( );

    mem_block_resize( nd, 16 );
    mem_block_resize( nd, 32 );
    mem_block_resize( nd, 63 );
    mem_block_resize( nd, 3 );

    mem_block_delete( nd );

    return 1;
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
