
#include <stdio.h>
#include <string.h>
#include <time.h>

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
    //printf( "free element: %p\n", elem );
}

void fake_freeing2( size_t *elem )
{
    printf( "free element: %u %p\n", *elem, elem );
}

void fake_freeing3( char *elem )
{
    printf( "free element: %c %c %c %p\n",
            elem[0], elem[1], elem[2], elem );
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


int cmp( int *l, int *r )
{
    static int i = 0;
    printf( "chek %u\n", i++ );
    return *l < *r ? -1 : *r < *l;
}

struct mm_block *pack_size( unsigned long size )
{
    struct mm_block *block = mm_block_new_reserved(sizeof( size ));
    if( block ) {
        if( size <= 0x7F ) {
            mm_block_push_back( block, (char)(size & 0x7F) );
        } else {
            while( size  ) {
                char next = (unsigned char)(size & 0x7F);
                next |= (( size >>= 7 ) ? 0x80 : 0x00 );
                mm_block_push_back( block, next );
            }
        }
    }
    return block;
}

unsigned long unpack_size( const char *data, size_t len )
{
    unsigned long res  = 0;
    unsigned shift     = 0;
    unsigned long next = 0;
    if( len ) do {
        next = *(unsigned char*)(data++);
        res |= ((next & 0x7F) << shift);
        shift += 7;
    } while ( next & 0x80 && --len );
    return res;
}

struct three_chars { char c1; char c2; char c3; char c4; };

int main( )
{
    goto deque_test;
    struct mm_block *pack = pack_size( 86942 );
    size_t packed = mm_block_size( pack );
    unsigned long unpacked = unpack_size( mm_block_begin( pack ), packed );

    int count = 0;
    int enf = mm_block_size( pack );
    for( ;count!=enf; ++count ) {
        unsigned char  x = *(unsigned char *)mm_block_at( pack, count );
        printf( " %u %x", x, x );
    }
    printf( " %u unpacked = %u %p\n", sizeof(long), unpacked, unpacked );

    struct mm_array *bin = mm_array_new( 3 );

    srand( time(NULL) );

    int i;
    for( i=10; i>0; i-- ) {
        printf( "insert\n" );
        char test[3] = { '1', '2', '3' };
        mm_array_push_back( bin, test );
    }

    printf( "top char = %u\n",
            MM_ARRAY_CAST_BEGIN( struct three_chars, bin ).c4 );

    mm_array_free2( bin, fake_freeing3 );

//    return 0;
//    goto AATREE;
deque_test:
    (void)(0);
    struct cnt_deque *cnd = cnt_deque_new2( sizeof(size_t), fake_freeing2 );

    size_t ci;
    for(ci=0; ci<100; ci++) {
        cnt_deque_push_back2( cnd, &ci, copy_element );
    }

    for(ci=0; ci<100; ci++) {
        cnt_deque_push_front2( cnd, &ci, copy_element );
    }

    printf( "size is %u \n", cnt_deque_size( cnd ) );

    while( !cnt_deque_empty( cnd ) ) {
        cnt_deque_pop_back( cnd );
    }
    printf( "size is %u \n", cnt_deque_size( cnd ) );
    printf( "------\n" );

    //return 0;
//    for(ci=0; ci<201; ci++) {
//        cnt_deque_pop_front( cnd );
//    }

//    for(ci=0; ci<100000000; ci++) {
//        size_t *top = (size_t *)cnt_deque_front( cnd );
//        cnt_deque_push_back( cnd, top );
//        cnt_deque_pop_front( cnd );
//    }

//    for(ci=0; ci<10000; ci++) {
//        cnt_deque_push_back2( cnd, &ci, copy_element );
//    }

    printf( "------\n" );
    cnt_deque_free( cnd );
//    while ( cnt_deque_size(cnd) ) {
//        cnt_deque_pop_back2( cnd, fake_pop );
//        if( cnt_deque_size(cnd) == 57 ) break;
//    }
//    //goto AATREE;

//    //return 0;
//AATREE:
//    (void)( 0 );
//    struct mm_array *arr = MM_ARRAY_CREATE3( 0, size_t, fake_freeing );

//    size_t aaa[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
//    mm_array_push_back3( arr, aaa, 10, copy_element );

//    size_t i;
//    for( i=0; i<10; ++i ) {
//        mm_array_push_front3( arr, &i, 1, copy_element );
//    }

//    size_t *back = (size_t *)mm_array_create_insertion( arr, 5, 3 );
//    back[0] = 666666;
//    back[1] = 777777;
//    back[2] = 888888;

//    back = (size_t *)mm_array_create_insertion( arr, 10, 3 );
//    back[0] = 666666;
//    back[1] = 777777;
//    back[2] = 888888;

//    back = (size_t *)mm_array_create_insertion( arr, 0, 3 );
//    back[0] = 666666;
//    back[1] = 777777;
//    back[2] = 888888;

//    mm_array_resize( arr, 10 );

//    mm_array_free( arr );

//    return 0;


//    printf( "" );
//    struct aa_tree *aat = aa_tree_new( );
//    aa_tree_set_free( aat, fake_freeing );

//    size_t k;

//    size_t max_count = 100;

//    for( k=0; k<max_count; k++ ) {
//        int res =  aa_tree_insert( aat, (void *)k );
//        printf( "first insert: %u, %d\n", k, res );
//    }

//    for( k=10; k<max_count-60; k++ ) {
//        int res = aa_tree_delete( aat, (void *)(k) );
//        printf( "free res: %u, %d\n", k, res );
//    }


//    for( k=0; k<max_count; k++ ) {
//        int res =  aa_tree_insert( aat, (void *)k );
//        printf( "second insert: %u, %d\n", k, res );
//    }

//    for( k=0; k<max_count; k++  ) {
//        if( !aa_tree_find( aat, (void *)k  ) )
//            printf( "%u is not found\n", k );
//    }



//    printf("aatree top level: %u, size: %u\n",
//           aa_tree_top_level( aat ), aa_tree_size( aat ));

//    struct aa_tree_iterator *iter = aa_tree_reverse_iterator_new( aat );

//    if( iter ) while ( !aa_tree_iterator_end( iter ) ) {
//        printf( "iterator data: %u\n", aa_tree_iterator_get( iter ) );
//        aa_tree_iterator_next( iter );
//    }

//    aa_tree_iterator_free( iter );
//    aa_tree_set_free( aat, aa_tree_fake_del );
//    aa_tree_free( aat );
////    //for( k=0; k<14; ++k )
////    aa_tree_insert( aat, "dfghdgh" );
////    aa_tree_insert( aat, "abct" );
////    aa_tree_insert( aat, "gdfg" );
////    aa_tree_insert( aat, "gdfg" );
////    aa_tree_insert( aat, "1234567890" );
////    aa_tree_insert( aat, "gdfg" );
////    aa_tree_insert( aat, "gdfaasdfasd" );


////    printf( "String is %d \n", (aa_tree_find( aat, "1234567890" ) != 0) );

////    //aa_tree_delete2( aat, 7, aa_tree_fake_del );

////    aa_tree_walk( aat, tree_walker, AA_WALK_ORDER );
////    printf( "\n" );
////    aa_tree_walk( aat, tree_walker, AA_WALK_REVERSE );
////    printf( "\n" );
////    aa_tree_walk( aat, tree_walker, AA_WALK_ROOT_LEFT );
////    printf( "\n" );
////    aa_tree_walk( aat, tree_walker, AA_WALK_ROOT_RIGHT );
////    printf( "\n" );

//    aa_tree_free( aat );
//    return 0;

//    struct mm_array *mar = MM_ARRAY_CREATE2( 10, int );

//    i = 0;
//    for( ;i!=10; ++i ) {
//        *((int *)mm_array_at( mar, i )) = i;
//    }

//    for( i=0;i!=10; ++i ) {
//        int j = *((int *)mm_array_at( mar, i )) ;
//        printf( "%d : %d \n", i, j );
//    }

//    mm_array_free( mar );

//    return 0;
//    struct bit_pack_data *bpd = bit_pack_new( );

//    bp_add_bits( bpd, 11, 11 );
//    bp_add_bits( bpd, 11, 11 );
//    bp_add_bits( bpd, 11, 11 );
//    bp_add_bits( bpd, 0xff, 6 );
//    bp_add_bits( bpd, 0, 1 );
//    bp_add_bits( bpd, 1, 1 );
//    bp_add_bits( bpd, 0, 1 );
//    bp_add_bits( bpd, 1, 1 );
//    bp_add_bits( bpd, 1, 32 );
//    bp_add_bits( bpd, 0xBAADFEED, 32 );

//    {
//        char data[100];
//        data[0] = (char)(bp_get_padd(bpd) & 0xff );
//        size_t copied = bp_copy_data( bpd, &data[1], bp_get_size( bpd ) );
//        size_t i;
//        printf( "copy_size: %u\n",  copied );
//        for( i=0; i != (copied+1); ++i ) {
//            char bb[9];
//            printf( "%d:\t%s\n", i, byte_to_(data[i], bb) );
//        }
//    }

//    printf( "padd\t%d\n", bp_get_padd( bpd ) );
//    printf("total: %d\n", bp_get_size( bpd ));

//    //bp_dump( bpd );

//    //struct bit_unpack_data *bup = bit_unpack_new();
//    //bu_assign( bup, "1234567890-", 11 );

//    struct bit_unpack_data *bup = bit_unpack_new2( "1234567890-", 11 );

//    while( !bu_eod( bup ) ) {
//        size_t val = 0;
//        unsigned got = bu_get_bits( bup, &val, 8 );
//        printf( "got: %u, next: %c  ", got, val );
//        size_dump_( val );
//    }

//    bit_pack_free( bpd );
//    bit_unpack_free( bup );

    return 0;
}
