
#include <stdio.h>
#include <string.h>

#include "bitpacker/bitpack.h"
#include "memory/mm-block.h"
#include "memory/mm-array.h"
#include "lists/linked-list.h"
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

int tree_walker( void *d )
{
    printf( " %u ", (size_t)d );
    return 1;
}

void aa_tree_fake_del( void *d )
{
    printf( "del %d\n", (int)( d ) );
}

void fake_freeing( size_t *elem )
{
    printf( "free element: %u\n", *elem );
}

void copy_element( size_t *new_place,
                   size_t *element, size_t element_size  )
{
    printf( "copy element: %u to %p, (%u)\n",
            *element, new_place, element_size );
    *new_place = *element;
}

int main( )
{
    goto AATREE;
    struct mm_array_data *arr = mm_array_create3( 0, size_t, fake_freeing );

    size_t i;
    for( i=0; i<100; ++i ) {
        mm_array_push_back3( arr, &i, copy_element );
    }

    mm_array_resize( arr, 10 );

    mm_array_free( arr );

    return 0;

AATREE:

    printf( "" );
    struct aa_tree *aat = aa_tree_new( );
    int k;

    aa_tree_set_free( aat, aa_tree_fake_del );

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

    aa_tree_free( aat );
    return 0;

    struct mm_array_data *mar = mm_array_create2( 10, int );

    i = 0;
    for( ;i!=10; ++i ) {
        *((int *)mm_array_at( mar, i )) = i;
    }

    for( i=0;i!=10; ++i ) {
        int j = *((int *)mm_array_at( mar, i )) ;
        printf( "%d : %d \n", i, j );
    }

    mm_array_free( mar );

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
