
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

struct prefix_info {
    int inf;
    const char *data;
};

struct prefix_info *info( const char * data )
{
    static int i = 0;
    printf( "create info: %i\n", ++i );

    struct prefix_info* n =
            (struct prefix_info *)malloc(sizeof(struct prefix_info));
    n->data = data;
    n->inf = i;
    return n;
}

void prefix_info_free( void *ptr )
{
    static int i = 0;
    struct prefix_info* n = (struct prefix_info*)ptr;
    printf( "free info: %i\n", n->inf );
    free( n );
}

static const char * cp_black           = "\x1b[30;1m";
static const char * cp_red             = "\x1b[31;1m";
static const char * cp_green           = "\x1b[32;1m";
static const char * cp_yellow          = "\x1b[33;1m";
static const char * cp_orange          = "\x1b[33;1m";
static const char * cp_blue            = "\x1b[34;1m";
static const char * cp_purple          = "\x1b[35;1m";
static const char * cp_vaaleansininen  = "\x1b[36;1m"; // :) light_blue
static const char * cp_white           = "\x1b[37;1m";

// color suffix
static const char * cs_stop            = "\x1b[0m";

#define COLOR( str ) str, strlen(str)

void fill_table( struct prefix_tree *trie )
{
    prefix_tree_insert_string( trie, "black", info(cp_black) );
    prefix_tree_insert_string( trie, "черный", info(cp_black ) );
    prefix_tree_insert_string( trie, "чёрный", info(cp_black ) );
    prefix_tree_insert_string( trie, "musta", info(cp_black ) );
    prefix_tree_insert_string( trie, "bla", info(cp_black ) );

    prefix_tree_insert_string( trie, "red", info(cp_red ) );
    prefix_tree_insert_string( trie, "красный", info(cp_red ) );
    prefix_tree_insert_string( trie, "puna", info(cp_red ) );

    prefix_tree_insert_string( trie, "green", info(cp_green ) );
    prefix_tree_insert_string( trie, "зелёный", info(cp_green ) );
    prefix_tree_insert_string( trie, "зеленый", info(cp_green ) );
    prefix_tree_insert_string( trie, "vihreä", info(cp_green ) );

    prefix_tree_insert_string( trie, "yellow", info(cp_yellow ) );
    prefix_tree_insert_string( trie, "желтый", info(cp_yellow ) );
    prefix_tree_insert_string( trie, "жёлтый", info(cp_yellow ) );
    prefix_tree_insert_string( trie, "Желтый", info(cp_yellow ) );
    prefix_tree_insert_string( trie, "Жёлтый", info(cp_yellow ) );
    prefix_tree_insert_string( trie, "keltainen", info(cp_yellow ) );

    prefix_tree_insert_string( trie, "orange", info(cp_orange ) );

    prefix_tree_insert_string( trie, "blue", info(cp_blue ) );
    prefix_tree_insert_string( trie, "синий", info(cp_blue ) );
    prefix_tree_insert_string( trie, "Синий", info(cp_blue ) );
    prefix_tree_insert_string( trie, "sininen", info(cp_blue ) );

    prefix_tree_insert_string( trie, "lightblue", info(cp_vaaleansininen ) );
    prefix_tree_insert_string( trie, "голубой", info(cp_vaaleansininen ) );
    prefix_tree_insert_string( trie, "vaaleansininen", info(cp_vaaleansininen ));

    prefix_tree_insert_string( trie, "purple",     info(cp_purple) );
    prefix_tree_insert_string( trie, "пурпурный",  info(cp_purple) );
    prefix_tree_insert_string( trie, "violetti",   info(cp_purple) );
    prefix_tree_insert_string( trie, "фиолет",   info(cp_purple) );
    prefix_tree_insert_string( trie, "Фиолет",   info(cp_purple) );

    prefix_tree_insert_string( trie, "white",       info(cp_white) );
    prefix_tree_insert_string( trie, "белый",       info(cp_white) );
    prefix_tree_insert_string( trie, "valkoinen",   info(cp_white) );

}

int main( )
{
    struct prefix_tree *trie = prefix_tree_new2( prefix_info_free );
    fill_table( trie );
    prefix_tree_insert_string( trie, "1234", info(cp_black) );

    struct mm_block *tmp_str = mm_block_new();

    size_t c = 0;

    const char *data = "black\nyellow\tred wings\nred-one\nvioletti\norange";
    size_t tmp_len = strlen(data);

    for( c=0; c<1; ++c ) {
        const char *p = data;
        size_t len = tmp_len;
        while ( len ) {
            const char *old_p = p;
            struct prefix_info *data = (struct prefix_info *)
                    prefix_tree_get_next( trie, (const void **)&p, &len );
            if( data ) {
                mm_block_concat( tmp_str, data->data, strlen(data->data) );
                mm_block_concat( tmp_str, old_p, p-old_p );
                mm_block_concat( tmp_str, cs_stop, strlen(cs_stop) );
           } else {
                mm_block_push_back( tmp_str, *p++ );
                len--;
            }
        }
    }

    mm_block_push_back( tmp_str, 0 );
    mm_block_reduce( tmp_str, 1 );

    printf( "%s\n", mm_block_begin( tmp_str ) );

    mm_block_free( tmp_str );
    prefix_tree_free( trie );

    return 0;
}
