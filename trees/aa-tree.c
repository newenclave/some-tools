
#include "aa-tree.h"

#define AA_BOTTOM_LEVEL_VALUE 1

int aa_tree_data_compare_default( const void *l, const void *r )
{
    return (l < r) ? -1 : (r < l);
}

struct aa_tree_node {
    struct aa_tree_node    *left_;
    struct aa_tree_node    *right_;
    union  node_data {
        void    *ptr_;
        int      number_;
    }                       data_;
    size_t                  level_;
};

typedef struct aa_tree_node *aa_tree_node_ptr;

struct aa_tree {
    aa_tree_data_compare cmp_;
    aa_tree_data_free    free_;
    struct aa_tree_node *root_;
    unsigned             count_;
};

struct aa_tree *aa_tree_new3 ( aa_tree_data_compare compare,
                               aa_tree_data_free free_call)
{
    struct aa_tree *new_tree =
            (struct aa_tree *)calloc( 1, sizeof(struct aa_tree) );
    if( new_tree ) {
        new_tree->cmp_  = compare;
        new_tree->free_ = free_call;
    }
    return new_tree;
}


struct aa_tree *aa_tree_new2( aa_tree_data_compare compare )
{
    return aa_tree_new3( compare, NULL );
}

struct aa_tree *aa_tree_new( )
{
    return aa_tree_new3( aa_tree_data_compare_default, NULL );
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
        *top      =  l;
        *left     =  t;
        t->left_  =  l->right_;
        l->right_ =  t;
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
        new_data->level_     = AA_BOTTOM_LEVEL_VALUE;
    }
    return new_data;
}

void *aa_tree_node_find( struct aa_tree_node *top, void *data,
                         aa_tree_data_compare cmp)
{
    if( !top ) {
        return NULL;
    } else {
        int cmp_res = cmp( data, top->data_.ptr_ );
        if( -1 == cmp_res ) {
            return aa_tree_node_find( top->left_, data, cmp );
        } else if( 1 == cmp_res  ) {
            return aa_tree_node_find( top->right_, data, cmp );
        }
    }
    return top->data_.ptr_;
}


void *aa_tree_find( struct aa_tree *aat, void *data )
{
    return aa_tree_node_find( aat->root_, data, aat->cmp_ );
}


int aa_tree_node_insert( aa_tree_node_ptr *top,
                         void *data,
                         aa_tree_data_compare  compare,
                         aa_tree_data_update update_call, int update)
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
        int cmp_res = compare( data, top_node->data_.ptr_ );

        if( -1 ==  cmp_res ) {
            result = aa_tree_node_insert( &top_node->left_,
                                          data, compare, update_call,
                                          update);

        } else if( 1 == cmp_res ) {
            result = aa_tree_node_insert( &top_node->right_,
                                          data, compare, update_call,
                                          update );
        } else {
            if( update && update_call) {
                if( update_call )
                    update_call( top_node->data_.ptr_, data );
                else
                    top_node->data_.ptr_ = data;
            }
        }

        if( result == 1 ) {
            *top = split(skew(top_node));
        }
    }
    return result;
}

int aa_tree_insert_or_update ( struct aa_tree *aat, void *data,
                               int update,
                               aa_tree_data_update update_call )
{
    int result = aa_tree_node_insert( &aat->root_, data,
                                       aat->cmp_, update_call, update );
    aat->count_ += (result == 1);
    return (result != -1);
}

int aa_tree_insert( struct aa_tree *aat, void *data )
{
    return aa_tree_insert_or_update(aat, data, 0, NULL);
}

int aa_tree_insert_update( struct aa_tree *aat, void *data,
                           aa_tree_data_update update_call )
{
    return aa_tree_insert_or_update(aat, data, 1, update_call);
}

int aa_tree_update( struct aa_tree *aat, void *data,
                    aa_tree_data_update update_call )
{
    return aa_tree_insert_or_update(aat, data, 1, update_call);
}

#define aa_tree_node_check_levels(t)                            \
         (t->right_ && (t->right_->level_ < (t->level_-1)))     \
            ||                                                  \
         (t->left_  && (t->left_->level_  < (t->level_-1)))


struct aa_tree_node *aa_tree_node_rebalance( struct aa_tree_node *t )
{
    if( aa_tree_node_check_levels( t ) ) {

        --t->level_;
        if( t->right_->level_ > t->level_ )
            t->right_->level_ = t->level_;

        t                 = skew ( t );
        t->right_         = skew ( t->right_ );
        t->right_->right_ = skew ( t->right_->right_ );

        t                 = split( t );
        t->right_         = split( t->right_ );

    }
    return t;
}

struct aa_tree_node *aa_tree_node_take_left( struct aa_tree_node * cur,
                                aa_tree_node_ptr *bup)
{
    if (NULL == cur->left_ ) {
        *bup = cur;
        return cur->right_;
    }

    cur->left_ = aa_tree_node_take_left(cur->left_, bup);

    return aa_tree_node_rebalance(cur);
}

struct aa_tree_node *aa_tree_node_del( struct aa_tree_node *deleted )
{
    struct aa_tree_node *res_node = NULL;
    if( NULL == deleted->left_ )
        res_node = deleted->right_;
    else if( NULL == deleted->right_ )
        res_node = deleted->left_;
    else {
        deleted->right_  = aa_tree_node_take_left( deleted->right_, &res_node );
        res_node->left_  = deleted->left_;
        res_node->right_ = deleted->right_;
        res_node->level_ = deleted->level_;
    }
    return res_node;
}

int aa_tree_node_delete( aa_tree_node_ptr *top,
                         void *data,
                         aa_tree_data_compare compare,
                         aa_tree_data_free    free_fun)
{
    int result = 0;
    struct aa_tree_node *t = *top;
    if( t ) {

        int cmp_res = compare( data, t->data_.ptr_ );

        if( -1 ==  cmp_res ) {
            result = aa_tree_node_delete( &t->left_, data, compare, free_fun );
        } else if( 1 == cmp_res ) {
            result = aa_tree_node_delete( &t->right_, data, compare, free_fun);
        } else {

            /// aa_tree_node_del

            struct aa_tree_node *tmp = NULL;
            if( NULL == t->left_ ) {
                tmp = t->right_;
            } else if( NULL == t->right_ ) {
                tmp = t->left_;
            } else {

                t->right_ = aa_tree_node_take_left( t->right_, &tmp );

                tmp->left_  = t->left_;
                tmp->right_ = t->right_;
                tmp->level_ = t->level_;
            }

            if( free_fun )
                free_fun( t->data_.ptr_ );
            free( t );
            result = 1;
            t = tmp;
        }
        *top = t;
    }
    return result;
}

int aa_tree_delete2( struct aa_tree *aat,
                     void *data, aa_tree_data_free free_fun )
{
    int res = aa_tree_node_delete( &aat->root_, data, aat->cmp_, free_fun );
    aat->count_ -= (res == 1);
    return res;
}

int aa_tree_delete( struct aa_tree *aat, void *data )
{
    return aa_tree_delete2( aat, data, aat->free_ );
}

size_t aa_tree_size( struct aa_tree *aat )
{
    return aat->count_;
}

void aa_tree_free_node( struct aa_tree_node *t, aa_tree_data_free free_fun )
{
    if( t ) {
        aa_tree_free_node( t->left_, free_fun );
        if( free_fun )
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
    aa_tree_free2(aat, aat->free_ );
}

int aa_tree_walk_ordered( struct aa_tree_node *t,
                         aa_tree_walker wlker, size_t *accum )
{
    if( t ) {
        if( 0 == aa_tree_walk_ordered( t->left_, wlker, accum ) ) return 0;
        ++(*accum);
        if( 0 == wlker( t->data_.ptr_ ) ) return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_ordered( t->right_, wlker, accum ) ) return 0;
        ++(*accum);
    }
    return 1;
}

int aa_tree_walk_reverse( struct aa_tree_node *t,
                         aa_tree_walker wlker, size_t *accum )
{
    if( t ) {
        if( 0 == aa_tree_walk_reverse( t->right_, wlker, accum ) ) return 0;
        ++(*accum);
        if( 0 == wlker( t->data_.ptr_ ) ) return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_reverse( t->left_, wlker, accum ) ) return 0;
        ++(*accum);
    }
    return 1;
}

int aa_tree_walk_root_left( struct aa_tree_node *t,
                            aa_tree_walker wlker, size_t *accum )
{
    if( t ) {
        if( 0 == wlker( t->data_.ptr_ ) ) return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_root_left( t->left_, wlker, accum ) ) return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_root_left( t->right_, wlker, accum ) ) return 0;
        ++(*accum);
    }
    return 1;

}

int aa_tree_walk_root_right( struct aa_tree_node *t,
                             aa_tree_walker wlker, size_t *accum )
{
    if( t ) {
        if( 0 == wlker( t->data_.ptr_ ) ) return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_root_right( t->right_, wlker, accum ) ) return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_root_right( t->left_, wlker, accum ) ) return 0;
        ++(*accum);
    }
    return 1;

}

void aa_tree_node_walk( struct aa_tree_node *t,
                       aa_tree_walker wlker,
                       enum aa_tree_directions direct,
                       size_t *accum )
{
    switch( direct ) {
    case AA_WALK_ORDER:
        aa_tree_walk_ordered( t, wlker, accum );
        break;
    case  AA_WALK_REVERSE:
        aa_tree_walk_reverse( t, wlker, accum );
        break;
    case  AA_WALK_ROOT_LEFT:
        aa_tree_walk_root_left( t, wlker, accum );
        break;
    case  AA_WALK_ROOT_RIGHT:
        aa_tree_walk_root_right( t, wlker, accum );
        break;
    }
}

size_t aa_tree_walk( struct aa_tree *aat, aa_tree_walker wlker,
                     enum aa_tree_directions direct )
{
    size_t res = 0;
    aa_tree_node_walk( aat->root_, wlker, direct, &res );
    return res;
}

void aa_tree_set_free( struct aa_tree *aat,
                       aa_tree_data_free free_call )
{
    aat->free_ = free_call;
}

void aa_tree_non_rec_walk( struct aa_tree *aat )
{
    // not implemented yet
}

