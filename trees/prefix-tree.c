#include "prefix-tree.h"
#include "../memory/mm-array.h"

enum prefix_tree_flags {
    PT_FLAG_FINAL = 1
};

struct prefix_tree_key_pair {
    union key_type {
        char    k8_;
    } key_;
    unsigned flags_;
    struct mm_array *next_keys_;
};

struct prefix_tree {
    struct prefix_tree_key_pair *root_;
};

static void pt_free_key_pair( struct prefix_tree_key_pair *kp )
{
    mm_array_free(kp->next_keys_);
}

static int prefix_tree_key_compare_8( struct prefix_tree_key_pair *l,
                                      struct prefix_tree_key_pair *r)
{
    return l->key_.k8_ < r->key_.k8_ ? -1 : r->key_.k8_ < l->key_.k8_;
}

struct prefix_tree *prefix_tree_new( )
{
    struct prefix_tree *new_tree =
            (struct prefix_tree *)malloc(sizeof(struct prefix_tree));
    if( new_tree ) {
        new_tree->root_ = NULL;
    }
    return new_tree;
}

void prefix_tree_free( struct prefix_tree *pt )
{
    if( pt ) {
        free(pt);
    }
}

