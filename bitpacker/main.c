
#include <stdio.h>

#include "bitpack.h"
#include "mem-block.h"

char * byte_to_( unsigned char b, char *storage )
{
    int i;
    for( i=8; i>0; --i ) {
        storage[8-i] = (b & (1 << (i-1))) ? '1' : '0';
    }
    storage[8] = '\0';
    return storage;
}


int main( )
{
    struct bit_pack_data *bpd = bp_new_bitpack_data( );

    bp_add_bits( bpd, 11, 11 );
    bp_add_bits( bpd, 11, 11 );
    bp_add_bits( bpd, 11, 11 );
    bp_add_bits( bpd, 0xff, 7 );

    {
        char data[100];
        size_t copied = bp_copy_data( bpd, data, 10 );
        size_t i;
        printf( "copy_size: %u\n",  copied );
        for( i=0; i!=copied; ++i ) {
            char bb[9];
            printf( "%d: %s\n", i, byte_to_(data[i], bb) );
        }
    }
    printf( "padd %d\n", bp_get_padd( bpd ) );

    //bp_dump( bpd );


    printf("total: %d\n", bp_get_size( bpd ));
    bp_delete_bitpack_data( bpd );

    return 0;
}
