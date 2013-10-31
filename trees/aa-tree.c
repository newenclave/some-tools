
#include "aa-tree.h"

#define BOTTOM_LEVEL_VALUE 0

int aa_tree_data_less_default( const void *l, const void *r )
{
    return (l < r);
}

struct aa_tree_node {
    struct aa_tree_node    *left_;
    struct aa_tree_node    *right_;
    union node_data {
        void               *ptr_;
        int                 number_;
    }                       data_;
    size_t                  level_;
};

typedef struct aa_tree_node *aa_tree_node_ptr;

struct aa_tree {
    aa_tree_data_less    less_;
    struct aa_tree_node *root_;
    size_t               count_;
};

struct aa_tree *aa_tree_new2( aa_tree_data_less less )
{
    struct aa_tree *new_tree =
            (struct aa_tree *)calloc( 1, sizeof(struct aa_tree) );
    if( new_tree ) {
        new_tree->less_ = less;
    }
    return new_tree;
}

struct aa_tree *aa_tree_new( )
{
    return aa_tree_new2( aa_tree_data_less_default );
}

struct aa_tree_node *skew( struct aa_tree_node *t )
{
    aa_tree_node_ptr *left = &(t->left_);
    struct aa_tree_node *l =   t->left_;

    if( (l != 0) && (t->level_ == l->level_) ) {
        *left     = t;
        t->left_  = l->right_;
        l->right_ = t;
        return l;
    }
    return t;
}

void skew0( aa_tree_node_ptr *top )
{
    struct aa_tree_node *t = *top;
    aa_tree_node_ptr *left = &(t->left_);
    struct aa_tree_node *l =   t->left_;

    if( l == 0 ) {
        return;
    }

    if( t->level_ == l->level_ ) {
        *top      = l;
        *left     = t;
        t->left_  = l->right_;
        l->right_ = t;
    }
}

struct aa_tree_node * split( struct aa_tree_node *t )
{
    aa_tree_node_ptr    *right = &(t->right_);
    struct aa_tree_node *r     =   t->right_;

    if( 0 == r || 0 == r->right_ ) {
        return t;
    }

    if( t->level_ == r->right_->level_ ) {
        *right    = t;

        t->right_ = r->left_;
        r->left_  = t;
        ++r->level_;
        return r;
    }
    return t;
}

void split0( aa_tree_node_ptr *top )
{
    struct aa_tree_node *t =      *top;
    aa_tree_node_ptr    *right = &(t->right_);
    struct aa_tree_node *r     =   t->right_;

    if( 0 == r || 0 == r->right_ ) {
        return;
    }

    if( t->level_ == r->right_->level_ ) {
        *top      = r;
        *right    = t;

        t->right_ = r->left_;
        r->left_  = t;
        ++r->level_;
    }
}

struct aa_tree_node *aa_tree_create_node( void *data )
{
    struct aa_tree_node *new_data =
            (struct aa_tree_node *)calloc(1, sizeof(struct aa_tree_node) );
    if( new_data ) {
        new_data->data_.ptr_ = data;
        new_data->level_ = BOTTOM_LEVEL_VALUE;
    }
    return new_data;
}

void *aa_tree_node_find( struct aa_tree_node *top, void *data,
                         aa_tree_data_less less)
{
    if( !top ) {
        return NULL;
    } else if( less( data, top->data_.ptr_ ) ) {
        return aa_tree_node_find( top->left_, data, less );
    } else if( less( top->data_.ptr_, data ) ) {
        return aa_tree_node_find( top->right_, data, less );
    } else {
        return top->data_.ptr_;
    }
}


void *aa_tree_find( struct aa_tree *aat, void *data )
{
    return aa_tree_node_find( aat->root_, data, aat->less_ );
}


int aa_tree_node_insert( aa_tree_node_ptr *top,
                         void *data, aa_tree_data_less less )
{
    struct aa_tree_node *top_node = *top;
    int result = 0;
    if( NULL == top_node ) {
        top_node = aa_tree_create_node( data );
        if( top_node ) {
            *top = top_node;
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if( less(data, top_node->data_.ptr_) ) {
            result = aa_tree_node_insert( &top_node->left_,  data, less );
        } else if( less(top_node->data_.ptr_, data) ) {
            result = aa_tree_node_insert( &top_node->right_, data, less );
        } else {
            result = 0;
        }

        *top = split(skew(top_node));
    }
    return result;
}


int aa_tree_insert( struct aa_tree *aat, void *data )
{
    int result = aa_tree_node_insert( &aat->root_, data, aat->less_ );
    aat->count_ += (result == 1);
    return result;
}

size_t aa_tree_size( struct aa_tree *aat )
{
    return aat->count_;
}

void aa_tree_free_node( struct aa_tree_node *t, aa_tree_data_free free_fun )
{
    if( t ) {
        aa_tree_free_node( t->left_, free_fun );
        free_fun( t->data_.ptr_ );
        aa_tree_free_node( t->right_, free_fun );
        free( t );
    }
}

void aa_tree_free2( struct aa_tree *aat, aa_tree_data_free free_fun)
{
    if( aat ) {
        aa_tree_free_node( aat->root_, free_fun );
        free( aat );
    }
}

void aa_tree_free( struct aa_tree *aat )
{
    aa_tree_free2(aat, free);
}

