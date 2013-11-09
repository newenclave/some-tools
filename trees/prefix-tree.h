#ifndef PREFIXTREE_20131109_H
#define PREFIXTREE_20131109_H

#include <stdlib.h>

struct prefix_tree;

struct prefix_tree *prefix_tree_new( );
void   prefix_tree_free( struct prefix_tree *pt );

int   prefix_tree_insert_8( struct prefix_tree *pt,
                            char *key, size_t length, void *value );

#endif // PREFIXTREE_20131109_H
