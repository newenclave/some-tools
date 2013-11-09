
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
#include "varints/base128.h"

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

int main( )
{

    struct mm_block *container = mm_block_new_reserved( 16 );
    srand( time(NULL) );
    size_t c = 0;

    for( c=0; c<50000; ++c ) {
        size_t r = rand( ) % 500;
        int res = b128_pack_append( r, container );
        printf( "%u=%u ", r, res );
    }

    size_t cs = mm_block_size(container);
    printf( "\nlen: %u\n\n\n", cs );

    void *b = mm_block_begin( container );
    const char *e = mm_block_end( container );

    while( b != e ) {
        size_t r = 0;
        void *bb = b;
        int res = b128_unpack_shift( &b, &cs, &r );
        printf( "%u=%u ", r, (b-bb) );
    }
    printf( "\nlen: %u\n\n\n", cs );

    mm_block_free( container );
    return 0;
}
