#include "prefix-tree.h"
#include "../memory/mm-array.h"

enum prefix_tree_flags {
    PT_FLAG_FINAL = 1
};

union pt_key_type {
    char  k8_;
    short k16_;
};

struct pt_key_pair {
    union pt_key_type   key_;
    unsigned short      flags_;
    struct prefix_tree *parent_;
    void               *data_;
    struct mm_array    *next_keys_;
};

typedef int (*pt_key_comparator)( const void *lptr, const void *rptr);
typedef void (*pt_key_setter)( struct pt_key_pair *e, const void *k);
typedef struct pt_key_pair *(*pt_element_getter)
                            ( struct mm_array *key_map, const void *k);

struct prefix_tree {
    struct mm_array       *root_keys_;
    prefix_tree_data_free  free_;
    size_t                 element_size_;
    pt_key_comparator      cmp_;
    pt_element_getter      get_;
    pt_key_setter          set_;
};


static int pt_key_compare_8( const void *lptr, const void *rptr)
{
    const struct pt_key_pair *l = (const struct pt_key_pair *)lptr;
    const struct pt_key_pair *r = (const struct pt_key_pair *)rptr;

    return l->key_.k8_ < r->key_.k8_ ? -1 : r->key_.k8_ < l->key_.k8_;
}

static void pt_key_set_8( struct pt_key_pair *e, const void *k )
{
    e->key_.k8_ = *((const char *)k);
}

static struct pt_key_pair *pt_get_element_8( struct mm_array *key_map,
                                             const void *k )
{
    struct pt_key_pair tmp;
    char c = *((const char *)k);
    tmp.key_.k8_ = c;
    struct pt_key_pair *res =
            (struct pt_key_pair *)mm_array_bin_find( key_map, &tmp,
                                                     pt_key_compare_8);
    return res;
}

/*
static int pt_key_compare_16( const void *lptr, const void *rptr)
{
    const struct pt_key_pair *l = (const struct pt_key_pair *)lptr;
    const struct pt_key_pair *r = (const struct pt_key_pair *)rptr;

    return l->key_.k16_ < r->key_.k16_ ? -1 : r->key_.k16_ < l->key_.k16_;
}

static struct pt_key_pair *pt_get_element_16(struct mm_array *key_map, short c)
{
    struct pt_key_pair tmp;
    tmp.key_.k16_ = c;
    struct pt_key_pair *res =
            (struct pt_key_pair *)mm_array_bin_find( key_map, &tmp,
                                                     pt_key_compare_16);
    return res;
}
*/

static void pt_free_key_pair( void *ptr )
{
    struct pt_key_pair *kp = (struct pt_key_pair *)(ptr);
    if( kp->next_keys_ ) {
        if( kp->parent_->free_ ) {
            size_t count = mm_array_size( kp->next_keys_ );
            struct pt_key_pair *next;
            while( count-- ) {
                next = (struct pt_key_pair *)mm_array_at(kp->next_keys_, count);
                if( next->data_ && (next->flags_ & PT_FLAG_FINAL))
                    kp->parent_->free_( next->data_ );
            }
        }
        mm_array_free(kp->next_keys_);
    }
}

static struct mm_array *pt_new_keys(  )
{
    struct mm_array *new_keys = mm_array_new(sizeof(struct pt_key_pair));
    if( new_keys ) {
        mm_array_set_free( new_keys, pt_free_key_pair );
    }
    return new_keys;
}

void pt_set_root8( struct prefix_tree *root )
{
    root->cmp_          = pt_key_compare_8;
    root->get_          = pt_get_element_8;
    root->set_          = pt_key_set_8;
    root->element_size_ = 1;
}

struct prefix_tree *prefix_tree_new2( prefix_tree_data_free free_call )
{
    struct prefix_tree *new_tree =
            (struct prefix_tree *)malloc(sizeof(struct prefix_tree));
    if( new_tree ) {
        if( (new_tree->root_keys_ = pt_new_keys( )) ) {
            pt_set_root8( new_tree );
            new_tree->free_ = free_call;
        } else {
            free( new_tree );
            new_tree = NULL;
        }
    }
    return new_tree;
}

struct prefix_tree *prefix_tree_new( )
{
    return prefix_tree_new2( NULL );
}

void prefix_tree_set_free( struct prefix_tree *pt,
                           prefix_tree_data_free free_call)
{
    pt->free_ = free_call;
}


void prefix_tree_free( struct prefix_tree *pt )
{
    if( pt ) {
        mm_array_free( pt->root_keys_ );
        free(pt);
    }
}

void prefix_tree_free2( struct prefix_tree *pt,
                        prefix_tree_data_free free_call)
{
    if( pt ) {
        prefix_tree_set_free( pt, free_call );
        prefix_tree_free( pt );
    }
}

static struct pt_key_pair *prefix_tree_next( const struct prefix_tree *pt,
                                             const void **stream,
                                             size_t *available,
                                             int greedy )

{
    const char *p      = *((const char **)stream);
    const char *p_last = p;
    size_t len         = *available;
    size_t len_last    = len;

    struct mm_array    *key_map  = pt->root_keys_;
    struct pt_key_pair *element  = NULL;
    struct pt_key_pair *result   = NULL;

    const size_t esize = pt->element_size_;

    while( key_map && len-- ) {

        element = pt->get_( key_map, p );
        p += esize;

        if( element && (element->flags_ & PT_FLAG_FINAL) ) {
            result   = element;
            len_last = len;
            p_last   = p;
            if( !greedy ) break;
        }

        key_map = element ? element->next_keys_ : NULL;
    }

    if( result ) {
        *stream = p_last;
        *available = len_last;
    }

    return result;
}

void *prefix_tree_get_next( const struct prefix_tree *pt,
                            const void **stream, size_t *available )
{
    struct pt_key_pair * res = prefix_tree_next( pt, stream, available, 1 );
    return res ? res->data_ : NULL;
}

int prefix_tree_insert( struct prefix_tree *pt,
                        const void *key, size_t length, void *data )
{
    struct mm_array *key_map = pt->root_keys_;
    int result = 1;
    const size_t esize = pt->element_size_;
    const char *next = (const char *)key;

    while( length-- && result ) {

        struct pt_key_pair *element = pt->get_( key_map, next );

        if( !element ) {
            struct pt_key_pair tmp;
            pt->set_( &tmp, next );
            tmp.flags_      = length ? 0 : PT_FLAG_FINAL;
            tmp.data_       = length ? NULL : data;
            tmp.next_keys_  = NULL;
            tmp.parent_     = pt;
            element = mm_array_bin_insert( key_map, &tmp, pt->cmp_ );
            result = (element != NULL);
        } else {
            if( 0 == length && !(element->flags_ & PT_FLAG_FINAL)) {
                element->data_   = data;
                element->flags_ |= PT_FLAG_FINAL;
                result = 1;
            }
        }
        if( element && length ) {
            if( !element->next_keys_ ) {
                element->next_keys_ = pt_new_keys( );
            }
            key_map = element->next_keys_;
            result = (key_map != NULL);
        }
        next += esize;
    }

    return result;
}
