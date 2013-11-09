
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
//    printf( "free element: %c %c %c %p\n",
//            elem[0], elem[1], elem[2], elem );
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

static const char * cp_black           = "\x1b[30;1m";
static const char * cp_red             = "\x1b[31;1m";
static const char * cp_green           = "\x1b[32;1m";
static const char * cp_yellow          = "\x1b[33;1m";
static const char * cp_blue            = "\x1b[34;1m";
static const char * cp_purple          = "\x1b[35;1m";
static const char * cp_vaaleansininen  = "\x1b[36;1m"; // :) light_blue
static const char * cp_white           = "\x1b[37;1m";

// color suffix
static const char * cs_stop            = "\x1b[0m";

#define COLOR( str ) str, strlen(str)

void fill_table( struct prefix_tree *trie )
{
    prefix_tree_insert( trie, COLOR( "black" ), cp_black );
    prefix_tree_insert( trie, COLOR( "черный"), cp_black );
    prefix_tree_insert( trie, COLOR( "чёрный"), cp_black );
    prefix_tree_insert( trie, COLOR( "musta"), cp_black );
    prefix_tree_insert( trie, COLOR( "bla"), cp_black );

    prefix_tree_insert( trie, COLOR( "red"), cp_red );
    prefix_tree_insert( trie, COLOR( "красный"), cp_red );
    prefix_tree_insert( trie, COLOR( "puna"), cp_red );

    prefix_tree_insert( trie, COLOR( "green"), cp_green );
    prefix_tree_insert( trie, COLOR( "зелёный"), cp_green );
    prefix_tree_insert( trie, COLOR( "зеленый"), cp_green );
    prefix_tree_insert( trie, COLOR( "vihreä"), cp_green );

    prefix_tree_insert( trie, COLOR( "yellow"), cp_yellow );
    prefix_tree_insert( trie, COLOR( "желтый"), cp_yellow );
    prefix_tree_insert( trie, COLOR( "жёлтый"), cp_yellow );
    prefix_tree_insert( trie, COLOR( "keltainen"), cp_yellow );

    prefix_tree_insert( trie, COLOR( "blue"), cp_blue );
    prefix_tree_insert( trie, COLOR( "синий"), cp_blue );
    prefix_tree_insert( trie, COLOR( "sininen"), cp_blue );

    prefix_tree_insert( trie, COLOR( "lightblue"), cp_vaaleansininen );
    prefix_tree_insert( trie, COLOR( "голубой"), cp_vaaleansininen );
    prefix_tree_insert( trie, COLOR( "vaaleansininen"), cp_vaaleansininen );

//    prefix_tree_insert( trie, "purple"),     5);
//    prefix_tree_insert( trie, "пурпурный"),  5);
//    prefix_tree_insert( trie, "violetti"),   5);

//    prefix_tree_insert( trie, "lightblue"),      6);
//    prefix_tree_insert( trie, "голубой"),        6);
//    prefix_tree_insert( trie, "vaaleansininen"), 6);

//    prefix_tree_insert( trie, "white"),       7);
//    prefix_tree_insert( trie, "белый"),       7);
//    prefix_tree_insert( trie, "valkoinen"),   7);

}

int main( )
{
    struct prefix_tree *trie = prefix_tree_new2( fake_freeing3 );
    fill_table( trie );
//    prefix_tree_insert( trie, COLOR( "black" ), cp_black );
//    prefix_tree_insert( trie, COLOR( "bla"), cp_black );
//    prefix_tree_insert( trie, COLOR( "blue"), cp_blue );

    size_t c = 0;

    const char *data = "black\nyellow\tred wings\n";
    size_t tmp_len = strlen(data);

    for( c=0; c<1; ++c ) {
        const char *p = data;
        size_t len = tmp_len;
        while ( len ) {
            char *old_p = p;
            char *data = prefix_tree_get_next( trie, &p, &len );
            if( data ) {
                printf( data );
                while( old_p != p )
                    printf( "%c", *old_p++ );
                printf( cs_stop );
            } else {
                printf( "%c", *p );
                len--;
                p++;
            }
        }
    }

    prefix_tree_free( trie );

    return 0;
}
