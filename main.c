
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bitpacker/bitpack.h"
#include "memory/mm-block.h"
#include "memory/mm-array.h"
#include "lists/linked-list.h"
#include "inc/struct-fields.h"
#include "trees/aa-tree.h"
#include "trees/prefix-tree.h"
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

    struct prefix_tree *trie = prefix_tree_new( );

    char test[] = "1234567890";

    size_t val  = 100;
    size_t val2 = 200;
    size_t val3 = 300;

    printf( "val2 %p\n", &val2 );

    int res; //=

    res = prefix_tree_insert( trie, "12", 2, &val3 );
    res = prefix_tree_insert( trie, "12345", 5, &val );
    res = prefix_tree_insert( trie, "67890", 5, &val2 );

    char *p     = test;
    size_t len  = strlen( test );
    size_t *data = prefix_tree_get_next( trie, &p, &len );

    printf( "data: %p %u %s\n", data, (data?*data:0), p );

    data = prefix_tree_get_next( trie, &p, &len );

    printf( "data2: %p %u %s\n", data, (data?*data:0), p );

    prefix_tree_free2( trie, fake_freeing2 );

    return 0;
}
