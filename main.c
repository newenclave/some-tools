
#include <stdio.h>

#include "bitpacker/bitpack.h"
#include "memory/mem-block.h"
#include "memory/mem-array.h"
#include "lists/list-work.h"
#include "inc/struct-fields.h"


char * byte_to_( unsigned char b, char *storage )
{
    int i;
    for( i=8; i>0; --i ) {
        storage[8-i] = (b & (1 << (i-1))) ? '1' : '0';
    }
    storage[8] = '\0';
    return storage;
}

void size_dump_( size_t b )
{
    char d[9];
    char *data = (char *)(&b);
    int i;
    for( i=0; i<sizeof( size_t ); ++i ) {
        printf( " %s", byte_to_( data[i], d ) );
    }
    printf( "\n" );
}

struct test_list {
    struct linked_list_header list;
    int data;
};

int list_printer( struct linked_list_header *lst )
{
    struct test_list *s = field_entry( lst, struct test_list, list );
    printf( "data is: %u\n", s->data );
    return 1;
}

int main( )
{

    struct test_list tl1 = {{0}, 0};
    struct test_list tl2 = {{0}, 0};
    struct test_list tl3 = {{0}, 0};
    struct test_list tl4 = {{0}, 0};

    struct test_list *tp = &tl1;

    tl1.data = 1;
    tl2.data = 2;
    tl3.data = 3;
    tl4.data = 4;

    linked_list_insert( &tl1, &tl2, list );
    linked_list_insert( &tl3, &tl4, list );

    linked_list_insert_list( &tl1, &tl3, list );

    linked_list_foreach( tp, list_printer );

    printf( "last data %u\n", field_entry( linked_list_last(&tl1.list), struct test_list, list )->data );

    printf( "Len: %u\n", linked_list_length( &tl1.list )  );

    return 0;

    struct mem_array_data *mar = mem_array_create2( 10, int );

    int i = 0;
    for( ;i!=10; ++i ) {
        *((int *)mem_array_at( mar, i )) = i;
    }

    for( i=0;i!=10; ++i ) {
        int j = *((int *)mem_array_at( mar, i )) ;
        printf( "%d : %d \n", i, j );
    }

    mem_array_free( mar );

    return 0;
    struct bit_pack_data *bpd = bit_pack_new( );

    bp_add_bits( bpd, 11, 11 );
    bp_add_bits( bpd, 11, 11 );
    bp_add_bits( bpd, 11, 11 );
    bp_add_bits( bpd, 0xff, 6 );
    bp_add_bits( bpd, 0, 1 );
    bp_add_bits( bpd, 1, 1 );
    bp_add_bits( bpd, 0, 1 );
    bp_add_bits( bpd, 1, 1 );
    bp_add_bits( bpd, 1, 32 );
    bp_add_bits( bpd, 0xBAADFEED, 32 );

    {
        char data[100];
        data[0] = (char)(bp_get_padd(bpd) & 0xff );
        size_t copied = bp_copy_data( bpd, &data[1], bp_get_size( bpd ) );
        size_t i;
        printf( "copy_size: %u\n",  copied );
        for( i=0; i != (copied+1); ++i ) {
            char bb[9];
            printf( "%d:\t%s\n", i, byte_to_(data[i], bb) );
        }
    }

    printf( "padd\t%d\n", bp_get_padd( bpd ) );
    printf("total: %d\n", bp_get_size( bpd ));

    //bp_dump( bpd );

    //struct bit_unpack_data *bup = bit_unpack_new();
    //bu_assign( bup, "1234567890-", 11 );

    struct bit_unpack_data *bup = bit_unpack_new2( "1234567890-", 11 );

    while( !bu_eod( bup ) ) {
        size_t val = 0;
        unsigned got = bu_get_bits( bup, &val, 8 );
        printf( "got: %u, next: %c  ", got, val );
        size_dump_( val );
    }

    bit_pack_free( bpd );
    bit_unpack_free( bup );

    return 0;
}
