
#include <stdio.h>
#include <string.h>

#include "bitpacker/bitpack.h"
#include "memory/mm-block.h"
#include "memory/mm-array.h"
#include "lists/linked-list.h"
#include "inc/struct-fields.h"
#include "trees/aa-tree.h"
#include "containers/cnt-deque.h"

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

int tree_walker( void *c )
{
    printf( "%s\n", c );
    return 1;
}

void aa_tree_fake_del( void *d )
{
    printf( "del %d\n", (int)( d ) );
}

void fake_freeing( size_t *elem )
{
    printf( "free element: %p\n", elem );
}

void fake_pop( size_t *elem )
{
    printf( "pop element: %u %p\n", *elem, elem );
}

void copy_element( size_t *new_place,
                   size_t *element, size_t element_size  )
{
    printf( "copy element: %u to %p, (%u)\n",
            *element, new_place, element_size );
    *new_place = *element;
}

//bool bin_search( std::vector<int> &data,
//                int val, size_t &pos )
//{
//    size_t right  = data.size( );
//    size_t left   = 0;
//    size_t middle = 0;
//    int cmp       = -1;
//    while( right > left ) {
//        middle  = left + ((right - left) >> 1);
//        cmp     = cmp_data( val, data[middle] );
//        if( cmp == 0 ) {
//            break;
//        } else if( cmp < 0 ) {
//            right = middle;
//        } else {
//            if( left == middle ) {
//                middle = left + 1;
//                break;
//            } else {
//                left  = middle;
//            }
//        }
//    }
//    pos = middle;
//    return (cmp == 0);
//}


//bool bin_search( std::vector<int> &data,
//                int val, size_t &pos )
//{
//    size_t right  =  data.size( );
//    size_t left   =  0;
//    size_t middle =  0;
//    int cmp       = -1;
//    while( (right != left) && (cmp != 0) ) {
//        middle = left + ((right - left) >> 1);
//        cmp    = cmp_data( val, data[middle] );
//        if( cmp != 0 ) {
//            if( cmp < 0 ) {
//                right = middle;
//            } else if( left == middle ) {
//                middle = left + 1;
//                right  = left;
//            } else {
//                left  = middle;
//            }
//        }
//    }
//    pos = middle;
//    return (cmp == 0);
//}

int main( )
{

    goto AATREE;
    struct cnt_deque *cnd = cnt_deque_new2( sizeof(size_t), fake_freeing );

    size_t ci;
    for(ci=0; ci<100; ci++) {
        cnt_deque_push_front2( cnd, &ci, copy_element );
    }
    for(ci=0; ci<100; ci++) {
        cnt_deque_push_back2( cnd, &ci, copy_element );
    }
    while ( cnt_deque_size(cnd) ) {
        cnt_deque_pop_back2( cnd, fake_pop );
        if( cnt_deque_size(cnd) == 57 ) break;
    }
    cnt_deque_free( cnd );
    //goto AATREE;

    //return 0;

    struct mm_array *arr = mm_array_create3( 0, size_t, fake_freeing );

    size_t aaa[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    mm_array_push_back3( arr, aaa, 10, copy_element );

    size_t i;
    for( i=0; i<10; ++i ) {
        mm_array_push_front3( arr, &i, 1, copy_element );
    }

    size_t *back = (size_t *)mm_array_create_insertion( arr, 5, 3 );
    back[0] = 666666;
    back[1] = 777777;
    back[2] = 888888;

    back = (size_t *)mm_array_create_insertion( arr, 10, 3 );
    back[0] = 666666;
    back[1] = 777777;
    back[2] = 888888;

    back = (size_t *)mm_array_create_insertion( arr, 0, 3 );
    back[0] = 666666;
    back[1] = 777777;
    back[2] = 888888;

    //mm_array_resize( arr, 10 );

    mm_array_free( arr );

    //return 0;

AATREE:

    printf( "" );
    struct aa_tree *aat = aa_tree_new( );
    aa_tree_set_free( aat, fake_freeing );

    size_t k;

    size_t max_count = 100;

    for( k=0; k<max_count; k++ ) {
        int res =  aa_tree_insert( aat, (void *)k );
        printf( "first insert: %u, %d\n", k, res );
    }

    for( k=10; k<max_count-60; k++ ) {
        int res = aa_tree_delete( aat, (void *)(k) );
        printf( "free res: %u, %d\n", k, res );
    }


    for( k=0; k<max_count; k++ ) {
        int res =  aa_tree_insert( aat, (void *)k );
        printf( "second insert: %u, %d\n", k, res );
    }

    for( k=0; k<max_count; k++  ) {
        if( !aa_tree_find( aat, (void *)k  ) )
            printf( "%u is not found\n", k );
    }



    printf("aatree top level: %u, size: %u\n",
           aa_tree_top_level( aat ), aa_tree_size( aat ));

    struct aa_tree_iterator *iter = aa_tree_reverse_iterator_create( aat );

    //if( iter ) while ( !aa_tree_iterator_end( iter ) ) {
        //printf( "iterator data: %u\n", aa_tree_iterator_get( iter ) );
        //aa_tree_iterator_next( iter );
    //}

    aa_tree_iterator_free( iter );

    //aa_tree_set_free( aat, aa_tree_fake_del );

//    //for( k=0; k<14; ++k )
//    aa_tree_insert( aat, "dfghdgh" );
//    aa_tree_insert( aat, "abct" );
//    aa_tree_insert( aat, "gdfg" );
//    aa_tree_insert( aat, "gdfg" );
//    aa_tree_insert( aat, "1234567890" );
//    aa_tree_insert( aat, "gdfg" );
//    aa_tree_insert( aat, "gdfaasdfasd" );


//    printf( "String is %d \n", (aa_tree_find( aat, "1234567890" ) != 0) );

//    //aa_tree_delete2( aat, 7, aa_tree_fake_del );

//    aa_tree_walk( aat, tree_walker, AA_WALK_ORDER );
//    printf( "\n" );
//    aa_tree_walk( aat, tree_walker, AA_WALK_REVERSE );
//    printf( "\n" );
//    aa_tree_walk( aat, tree_walker, AA_WALK_ROOT_LEFT );
//    printf( "\n" );
//    aa_tree_walk( aat, tree_walker, AA_WALK_ROOT_RIGHT );
//    printf( "\n" );

    aa_tree_free( aat );
    return 0;

    struct mm_array *mar = mm_array_create2( 10, int );

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
