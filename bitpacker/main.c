
#include <stdio.h>

#include "bitpack.h"
#include "mem-block.h"

int main( )
{
    struct mem_block_data *mb = mem_block_new( );

    int i=0;
    for( ;i<100; ++i) {
        mem_block_push_back( mb, '1' );
        printf( "capa: %u, s %u\n", mem_block_capacity(mb), mem_block_size(mb) );
    }
    mem_block_push_back( mb, 0 );

    printf( " data %s\n", (char *)mem_block_data( mb ) );

    mem_block_delete( mb );

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
