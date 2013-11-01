
#include <stdio.h>

#include "bitpacker/bitpack.h"
#include "memory/mem-block.h"
#include "memory/mem-array.h"
#include "lists/list-work.h"
#include "inc/struct-fields.h"
#include "trees/aa-tree.h"


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

int tree_walker( void *d )
{
    printf( " %u ", (unsigned)d );
    return 1;
}

void aa_tree_fake_del( void *d )
{
    printf( "del %d\n", (int)( d ) );
}

int main( )
{

    struct aa_tree *aat = aa_tree_new( );
    int k;

    //for( k=0; k<14; ++k )
    for( k=15; k>=0; --k )
        aa_tree_insert( aat, k );

    aa_tree_delete2( aat, 7, aa_tree_fake_del );

    aa_tree_walk( aat, tree_walker, AA_WALK_ORDER );
    printf( "\n" );
    aa_tree_walk( aat, tree_walker, AA_WALK_REVERSE );
    printf( "\n" );
    aa_tree_walk( aat, tree_walker, AA_WALK_ROOT_LEFT );
    printf( "\n" );
    aa_tree_walk( aat, tree_walker, AA_WALK_ROOT_RIGHT );
    printf( "\n" );

    aa_tree_free2( aat, aa_tree_fake_del );
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
