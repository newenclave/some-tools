#if !defined(AA_TREE_20133110_H)
#define AA_TREE_20133110_H

#include <stdlib.h>

struct aa_tree;
struct aa_tree_iterator;

typedef int  (* aa_tree_data_compare)( const void *l, const void *r );
typedef void (* aa_tree_data_free)( void *data );

typedef int  (* aa_tree_walker)( void *data );

enum aa_tree_directions {
     AA_WALK_ORDER      = 0
    ,AA_WALK_REVERSE    = 1
    ,AA_WALK_ROOT_LEFT  = 2
    ,AA_WALK_ROOT_RIGHT = 3
};

struct aa_tree *aa_tree_new  ( );
struct aa_tree *aa_tree_new2 ( aa_tree_data_compare compare );
struct aa_tree *aa_tree_new3 ( aa_tree_data_compare compare,
                               aa_tree_data_free free_call);

void            aa_tree_set_free( struct aa_tree *aat,
                                  aa_tree_data_free free_call );

void            aa_tree_free ( struct aa_tree *aat );
void            aa_tree_free2( struct aa_tree *aat, aa_tree_data_free free_fun);

int             aa_tree_insert ( struct aa_tree *aat, void *data );
void           *aa_tree_insert_get ( struct aa_tree *aat, void *data );
void           *aa_tree_find   ( struct aa_tree *aat, void *data );

size_t          aa_tree_size     ( const struct aa_tree *aat );
unsigned        aa_tree_top_level(const struct aa_tree *aat);

int             aa_tree_delete ( struct aa_tree *aat, void *data );
int             aa_tree_delete2( struct aa_tree *aat,
                                 void *data, aa_tree_data_free free_fun );

size_t          aa_tree_walk   ( struct aa_tree *aat,
                                 aa_tree_walker wlker,
                                 enum aa_tree_directions direction );

struct aa_tree_iterator
        *aa_tree_iterator_new( const struct aa_tree *aat );
struct aa_tree_iterator
        *aa_tree_reverse_iterator_new(const struct aa_tree *aat);
struct aa_tree_iterator
        *aa_tree_iterator_clone( const struct aa_tree_iterator *iter );

void    aa_tree_iterator_free( struct aa_tree_iterator *iter );
int     aa_tree_iterator_end( struct aa_tree_iterator *iter );
int     aa_tree_iterator_next( struct aa_tree_iterator *iter );
void   *aa_tree_iterator_get( struct aa_tree_iterator *iter );

#endif
