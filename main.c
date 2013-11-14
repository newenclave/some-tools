
#include <stdio.h>
#include <stdint.h>

#include "trees/prefix-tree.h"
#include "trees/aa-tree.h"
#include "memory/mm-block.h"
#include "varints/zig-zag.h"
#include "varints/base128.h"
#include "charset/cs-utf8.h"

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
    printf( "free info: %i %i\n", n->inf, ++i );
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
//static const char * cs_stop            = "\033[B";
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

    prefix_tree_insert_string( trie, "wings", info(cp_green ) );
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
    prefix_tree_insert_string( trie, "one", info(cp_vaaleansininen ));

    prefix_tree_insert_string( trie, "purple",     info(cp_purple) );
    prefix_tree_insert_string( trie, "пурпурный",  info(cp_purple) );
    prefix_tree_insert_string( trie, "violetti",   info(cp_purple) );
    prefix_tree_insert_string( trie, "фиолет",   info(cp_purple) );
    prefix_tree_insert_string( trie, "Фиолет",   info(cp_purple) );

    prefix_tree_insert_string( trie, "white",       info(cp_white) );
    prefix_tree_insert_string( trie, "белый",       info(cp_white) );
    prefix_tree_insert_string( trie, "valkoinen",   info(cp_white) );

}

static void *void_ptr_copy(void *new_place, const void *element, size_t es )
{
    (void)(es);
    *((size_t *)new_place) = *((size_t *)element);
    return new_place;
}

struct test {
    size_t i;
    size_t j;
};

void pack_string( const char *str, struct mm_block *mem )
{
    size_t len = strlen( str );
    b128_pack_append( len, mem );
    mm_block_concat( mem, str, len );
}

void save_to_file( struct mm_block *mem, const char *filename )
{
    FILE *f = fopen( filename, "wb" );
    if( f ) {
        fwrite( mm_block_begin(mem), 1, mm_block_size( mem ), f );
        fclose(f);
    }
}

int main( )
{

    printf( "%ul, %ul\n", 234234234 % 0x40, 234234234 & 0x3F );

    char test[10];
    size_t avail = 10;
    size_t res = cs_ucs4_to_utf8( 4000, test, 10 );
    res = cs_utf8_to_ucs4( test, 10, &avail );

    return 0;
    size_t it = 0;
    struct aa_tree *aat = aa_tree_new(  );

    for( it=0; it<100; it++ )
        aa_tree_insert( aat, (void *)it );

    struct aa_tree_iterator *iter = aa_tree_iterator_new( aat );
    struct aa_tree_iterator *iter2;
    while( !aa_tree_iterator_end( iter ) ) {
        size_t ig = (size_t)aa_tree_iterator_get( iter );
        printf( "iter next %lu\n", ig );
        aa_tree_iterator_next( iter );
        if( ig == 50 )
            iter2 = aa_tree_iterator_clone( iter );
    }
    printf( "========\n" );
    while( !aa_tree_iterator_end( iter2 ) ) {
        size_t ig = (size_t)aa_tree_iterator_get( iter2 );
        printf( "iter next %lu\n", ig );
        aa_tree_iterator_next( iter2 );
    }

    aa_tree_free( aat );
    aa_tree_iterator_free( iter );
    aa_tree_iterator_free( iter2 );

    return 0;

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

    printf( "%s\n", mm_block_const_begin( tmp_str ) );

    mm_block_free( tmp_str );
    prefix_tree_free( trie );

    return 0;
}
