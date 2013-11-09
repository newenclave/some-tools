#include "prefix-tree.h"
#include "../memory/mm-array.h"

enum prefix_tree_flags {
    PT_FLAG_FINAL = 1
};

struct pt_key_pair {

    union key_type {
        char k8_;
    } key_;

    unsigned flags_;
    void    *value_;
    struct mm_array *next_keys_;
};

struct prefix_tree {
    struct mm_array *root_keys_;
};

static void pt_free_key_pair( void *ptr )
{
    struct pt_key_pair *kp = (struct pt_key_pair *)(ptr);
    mm_array_free(kp->next_keys_);
}

static int pt_key_compare_8( const void *lptr, const void *rptr)
{
    const struct pt_key_pair *l = (const struct pt_key_pair *)lptr;
    const struct pt_key_pair *r = (const struct pt_key_pair *)rptr;

    return l->key_.k8_ < r->key_.k8_ ? -1 : r->key_.k8_ < l->key_.k8_;
}

static struct mm_array *pt_new_keys(  )
{
    struct mm_array *new_keys = mm_array_new(sizeof(struct pt_key_pair));
    if( new_keys ) {
        mm_array_set_free( new_keys, pt_free_key_pair );
    }
    return new_keys;
}

struct prefix_tree *prefix_tree_new( )
{
    struct prefix_tree *new_tree =
            (struct prefix_tree *)malloc(sizeof(struct prefix_tree));
    if( new_tree ) {
        if( !(new_tree->root_keys_ = pt_new_keys( )) ) {
            free( new_tree );
            new_tree = NULL;
        }
    }
    return new_tree;
}

void prefix_tree_free( struct prefix_tree *pt )
{
    if( pt ) {
        mm_array_free( pt->root_keys_ );
        free(pt);
    }
}

static struct pt_key_pair *pt_get_element_8( struct mm_array *key_map, char c )
{
    struct pt_key_pair tmp;
    tmp.key_.k8_ = c;

    struct pt_key_pair *res =
            (struct pt_key_pair *)mm_array_bin_find( key_map, &tmp,
                                                     pt_key_compare_8);
    return res;
}


struct pt_key_pair *prefix_tree_next_8( const struct prefix_tree *pt,
                                   char **stream, size_t *length, int greedly )

{
    char *p         = *stream;
    char *p_last    = p;
    char next       = 0;
    size_t len      = *length;
    size_t last_len = len;

    struct pt_key_pair *result   = NULL;
    struct mm_array    *key_map  = pt->root_keys_;

    while( key_map && len-- ) {
        next = *p++;
        struct pt_key_pair *element = pt_get_element_8( key_map, next );
        if( element ) {
            if( element->flags_ & PT_FLAG_FINAL ) {
                result   = element;
                last_len = len;
                p_last   = p;
                if( !greedly )
                    key_map = NULL;
            } else {
                key_map = element->next_keys_;
            }
        } else {
            key_map = NULL;
        }
    }

    if( result ) {
        *stream = p_last;
        *length = last_len;
    }

    return result;
}

void *prefix_tree_get_next_8( const struct prefix_tree *pt,
                          char **stream, size_t *length )
{
    struct pt_key_pair * res = prefix_tree_next_8( pt, stream, length, 1 );
    return res ? res->value_ : NULL;
}

int prefix_tree_insert_8( struct prefix_tree *pt,
                          char *key, size_t length, void *value )
{
    struct mm_array *key_map = pt->root_keys_;
    int result = 1;
    char next  = 0;

    while( length-- && result ) {

        next = *key++;
        struct pt_key_pair *element = pt_get_element_8( key_map, next );

        if( !element ) {
            struct pt_key_pair tmp;
            tmp.key_.k8_   = next;
            tmp.flags_     = length ? 0 : PT_FLAG_FINAL;
            tmp.value_     = length ? NULL : value;
            tmp.next_keys_ = NULL;
            element = mm_array_bin_insert( key_map, &tmp, pt_key_compare_8 );
            result = (element != NULL);
        }

        if( element ) {
            if( length ) {
                if( !element->next_keys_ ) {
                    element->next_keys_ = pt_new_keys( );
                }
                key_map = element->next_keys_;
                result = (key_map != NULL);
            } else {
                if( !(element->flags_ & PT_FLAG_FINAL) ) {
                    element->value_  = value;
                    element->flags_ |= PT_FLAG_FINAL;
                    result = 1;
                }
            }
        }

    }

    return result;
}
