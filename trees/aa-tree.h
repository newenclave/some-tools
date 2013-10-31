#if !defined(AA_TREE_20133110_H)
#define AA_TREE_20133110_H

#include <stdlib.h>

typedef int  (* aa_tree_data_less)( const void *l, const void *r );
typedef void (* aa_tree_data_free)( void *data );

struct aa_tree;

struct aa_tree *aa_tree_new  ( );
struct aa_tree *aa_tree_new2 ( aa_tree_data_less less );
void            aa_tree_free ( struct aa_tree *aat );
void            aa_tree_free2( struct aa_tree *aat, aa_tree_data_free free_fun);

int             aa_tree_insert( struct aa_tree *aat, void *data );
void           *aa_tree_find( struct aa_tree *aat, void *data );
size_t          aa_tree_size( struct aa_tree *aat );
int             aa_tree_delete( struct aa_tree *aat, void *data );
int             aa_tree_delete2( struct aa_tree *aat,
                                 void *data, aa_tree_data_free free_fun );

#endif
