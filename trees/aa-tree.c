
#include "aa-tree.h"
#include "containers/cnt-deque.h"

#define AA_BOTTOM_LEVEL_VALUE 1

enum aa_tree_link_position {
     AA_LINK_LEFT   = 0
    ,AA_LINK_RIGHT  = 1
};

struct aa_tree_node {
    struct aa_tree_node    *links_[2];
    union  node_data {
        void    *ptr_;
        char    *str_;
        size_t   number_;
    }                       data_;
    size_t                  level_;
};

typedef struct aa_tree_node *aa_tree_node_ptr;

struct aa_tree {
    aa_tree_data_compare cmp_;
    aa_tree_data_free    free_;
    struct aa_tree_node *root_;
    size_t               count_;
};

struct aa_tree_iterator
{
    const struct aa_tree      *parent_;
    struct       cnt_deque    *stack_;
    struct       aa_tree_node *current_;
    short        push_side_;
    short        next_side_;
};

static int aa_tree_data_compare_default( const void *l, const void *r )
{
    return (l < r) ? -1 : (r < l);
}

static void *aa_tree_malloc(size_t size)
{
    return malloc( size );
}

static void aa_tree_freemem(void *ptr)
{
    free(ptr);
}

struct aa_tree *aa_tree_new3 ( aa_tree_data_compare compare,
                               aa_tree_data_free free_call)
{
    struct aa_tree *new_tree =
            (struct aa_tree *)aa_tree_malloc( sizeof(struct aa_tree) );
    if( new_tree ) {
        new_tree->root_  = NULL;
        new_tree->count_ = 0;
        new_tree->cmp_   = compare;
        new_tree->free_  = free_call;
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

static struct aa_tree_node *skew( struct aa_tree_node *t )
{
    aa_tree_node_ptr *left = &(t->links_[AA_LINK_LEFT]);
    struct aa_tree_node *l =   t->links_[AA_LINK_LEFT];

    if( (l != 0) && (t->level_ == l->level_) ) {
        *left     = t;
        t->links_[AA_LINK_LEFT]  = l->links_[AA_LINK_RIGHT];
        l->links_[AA_LINK_RIGHT] = t;
        return l;
    }
    return t;
}

/*
static void skew0( aa_tree_node_ptr *top )
{
    struct aa_tree_node *t = *top;
    aa_tree_node_ptr *left = &(t->links_[AA_LINK_LEFT]);
    struct aa_tree_node *l =   t->links_[AA_LINK_LEFT];

    if( l == 0 ) {
        return;
    }

    if( t->level_ == l->level_ ) {
        *top      =  l;
        *left     =  t;
        t->links_[AA_LINK_LEFT]  =  l->links_[AA_LINK_RIGHT];
        l->links_[AA_LINK_RIGHT] =  t;
    }
}
*/

static struct aa_tree_node * split( struct aa_tree_node *t )
{
    aa_tree_node_ptr    *right = &(t->links_[AA_LINK_RIGHT]);
    struct aa_tree_node *r     =   t->links_[AA_LINK_RIGHT];

    if( 0 == r || 0 == r->links_[AA_LINK_RIGHT] ) {
        return t;
    }

    if( t->level_ == r->links_[AA_LINK_RIGHT]->level_ ) {
        *right    = t;

        t->links_[AA_LINK_RIGHT] = r->links_[AA_LINK_LEFT];
        r->links_[AA_LINK_LEFT]  = t;
        ++r->level_;
        return r;
    }
    return t;
}

/*
static void split0( aa_tree_node_ptr *top )
{
    struct aa_tree_node *t =      *top;
    aa_tree_node_ptr    *right = &(t->links_[AA_LINK_RIGHT]);
    struct aa_tree_node *r     =   t->links_[AA_LINK_RIGHT];

    if( 0 == r || 0 == r->links_[AA_LINK_RIGHT] ) {
        return;
    }

    if( t->level_ == r->links_[AA_LINK_RIGHT]->level_ ) {
        *top      = r;
        *right    = t;

        t->links_[AA_LINK_RIGHT] = r->links_[AA_LINK_LEFT];
        r->links_[AA_LINK_LEFT]  = t;
        ++r->level_;
    }
}
*/

struct aa_tree_node *aa_tree_create_node( void *data )
{
    struct aa_tree_node *new_data = (struct aa_tree_node *)
            aa_tree_malloc( sizeof(struct aa_tree_node) );
    if( new_data ) {
        new_data->links_[0] = new_data->links_[1] = NULL;
        new_data->data_.ptr_    = data;
        new_data->level_        = AA_BOTTOM_LEVEL_VALUE;
    }
    return new_data;
}

void *aa_tree_node_find( struct aa_tree_node *top, void *data,
                         aa_tree_data_compare cmp)
{
    void *res = NULL;
    if( top ) {
        int cmp_res = cmp( data, top->data_.ptr_ );
        if( 0 == cmp_res ) {
            res = top->data_.ptr_;
        } else {
            res = aa_tree_node_find( top->links_[cmp_res > 0], data, cmp );
        }
    }
    return res;
}


void *aa_tree_find( struct aa_tree *aat, void *data )
{
    return aa_tree_node_find( aat->root_, data, aat->cmp_ );
}


int aa_tree_node_insert( aa_tree_node_ptr *top,
                         void *data,
                         aa_tree_node_ptr *inserted,
                         aa_tree_data_compare  compare)
{
    struct aa_tree_node *top_node = *top;
    int res = 0;
    if( NULL == top_node ) {
        top_node = aa_tree_create_node( data );
        if( top_node ) {
            *top = *inserted = top_node;
            res = 1;
        } else {
            *inserted = NULL;
            res = 0;
        }
    } else {
        int cmp_res = compare( data, top_node->data_.ptr_ );
        if( 0 == cmp_res ) {
            res       = 2;
            *inserted = top_node;
        } else {
            res = aa_tree_node_insert( &top_node->links_[cmp_res > 0],
                                            data, inserted,
                                            compare);
        }

        if( res == 1 ) {
            *top = split(skew(top_node));
        }
    }
    return res;
}

void *aa_tree_insert_get ( struct aa_tree *aat, void *data )
{
    struct aa_tree_node *new_data = NULL;
    int res = aa_tree_node_insert( &aat->root_, data, &new_data, aat->cmp_ );
    aat->count_ += (res == 1);
    return new_data ? new_data->data_.ptr_ : NULL;
}

int aa_tree_insert( struct aa_tree *aat, void *data )
{
    struct aa_tree_node *new_data = NULL;
    int res = aa_tree_node_insert( &aat->root_, data, &new_data, aat->cmp_ );
    aat->count_ += (res == 1);
    return res;
}

#define aa_tree_node_check_levels(t)                                   \
         (t->links_[AA_LINK_RIGHT] &&                                  \
             (t->links_[AA_LINK_RIGHT]->level_ < (t->level_-1)))       \
            ||                                                         \
         (t->links_[AA_LINK_LEFT]  &&                                  \
            (t->links_[AA_LINK_LEFT]->level_  < (t->level_-1)))

struct aa_tree_node *aa_tree_node_rebalance( struct aa_tree_node *t )
{
    if( aa_tree_node_check_levels( t ) ) {

        --t->level_;
        if( t->links_[AA_LINK_RIGHT]->level_ > t->level_ )
            t->links_[AA_LINK_RIGHT]->level_ = t->level_;

        t                        = skew ( t );
        t->links_[AA_LINK_RIGHT] = skew ( t->links_[AA_LINK_RIGHT] );
        t->links_[AA_LINK_RIGHT]
         ->links_[AA_LINK_RIGHT] = skew ( t->links_[AA_LINK_RIGHT]
                                           ->links_[AA_LINK_RIGHT] );
        t                        = split( t );
        t->links_[AA_LINK_RIGHT] = split( t->links_[AA_LINK_RIGHT] );
    }
    return t;
}

struct aa_tree_node *aa_tree_node_take_left( struct aa_tree_node * cur,
                                aa_tree_node_ptr *bup)
{
    if (NULL == cur->links_[AA_LINK_LEFT] ) {
        *bup = cur;
        return cur->links_[AA_LINK_RIGHT];
    }

    cur->links_[AA_LINK_LEFT] =
            aa_tree_node_take_left(cur->links_[AA_LINK_LEFT], bup);

    return aa_tree_node_rebalance(cur);
}

/*
static struct aa_tree_node *aa_tree_node_del( struct aa_tree_node *del )
{
    struct aa_tree_node *res_node = NULL;
    if( NULL == del->links_[AA_LINK_LEFT] ) {

        res_node = del->links_[AA_LINK_RIGHT];

    } else if( NULL == del->links_[AA_LINK_RIGHT] ) {

        res_node = del->links_[AA_LINK_LEFT];

    } else {

        del->links_[AA_LINK_RIGHT]  =
                aa_tree_node_take_left( del->links_[AA_LINK_RIGHT], &res_node );
        res_node->links_[AA_LINK_LEFT]  = del->links_[AA_LINK_LEFT];
        res_node->links_[AA_LINK_RIGHT] = del->links_[AA_LINK_RIGHT];
        res_node->level_ = del->level_;
    }
    return res_node;
}
*/

int aa_tree_node_delete( aa_tree_node_ptr *top,
                         void *data,
                         aa_tree_data_compare compare,
                         aa_tree_data_free    free_fun)
{
    int result = 0;
    struct aa_tree_node *t = *top;
    if( t ) {

        int cmp_res = compare( data, t->data_.ptr_ );

        if( 0 == cmp_res ) {

            /// aa_tree_node_del

            struct aa_tree_node *tmp = NULL;
            if( NULL == t->links_[AA_LINK_LEFT] ) {

                tmp = t->links_[AA_LINK_RIGHT];

            } else if( NULL == t->links_[AA_LINK_RIGHT] ) {

                tmp = t->links_[AA_LINK_LEFT];

            } else {

                t->links_[AA_LINK_RIGHT] =
                        aa_tree_node_take_left(t->links_[AA_LINK_RIGHT], &tmp);

                tmp->links_[AA_LINK_LEFT]  = t->links_[AA_LINK_LEFT];
                tmp->links_[AA_LINK_RIGHT] = t->links_[AA_LINK_RIGHT];
                tmp->level_                = t->level_;
            }

            if( free_fun )
                free_fun( t->data_.ptr_ );
            aa_tree_freemem( t );
            result = 1;
            t = tmp;

        } else {
            result = aa_tree_node_delete( &t->links_[cmp_res > 0],
                                          data, compare, free_fun );
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

size_t aa_tree_size( const struct aa_tree *aat )
{
    return aat->count_;
}

unsigned aa_tree_top_level( const struct aa_tree *aat)
{
    if( aat->root_ ) return aat->root_->level_;
    else return 0;
}

void aa_tree_free_node( struct aa_tree_node *t, aa_tree_data_free free_fun )
{
    if( t ) {
        aa_tree_free_node( t->links_[AA_LINK_LEFT], free_fun );
        if( free_fun )
            free_fun( t->data_.ptr_ );
        aa_tree_free_node( t->links_[AA_LINK_RIGHT], free_fun );
        aa_tree_freemem( t );
    }
}

void aa_tree_free2( struct aa_tree *aat, aa_tree_data_free free_fun)
{
    if( aat ) {
        aa_tree_free_node( aat->root_, free_fun );
        aa_tree_freemem( aat );
    }
}

void aa_tree_free( struct aa_tree *aat )
{
    if( aat ) aa_tree_free2( aat, aat->free_ );
}

int aa_tree_walk_ordered( struct aa_tree_node *t, int order,
                          aa_tree_walker wlker,
                          size_t *accum )
{
    if( t ) {
        if( 0 == aa_tree_walk_ordered(t->links_[order!=0], order, wlker, accum))
            return 0;
        ++(*accum);
        if( 0 == wlker( t->data_.ptr_ ) ) return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_ordered(t->links_[order==0], order, wlker, accum))
            return 0;
        ++(*accum);
    }
    return 1;
}

int aa_tree_walk_root( struct aa_tree_node *t, int order,
                            aa_tree_walker wlker, size_t *accum )
{
    if( t ) {
        if( 0 == wlker( t->data_.ptr_ ) ) return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_root( t->links_[order!=0], order, wlker, accum ) )
            return 0;
        ++(*accum);
        if( 0 == aa_tree_walk_root( t->links_[order==0], order, wlker, accum ) )
            return 0;
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
        aa_tree_walk_ordered( t, AA_LINK_LEFT, wlker, accum );
        break;
    case  AA_WALK_REVERSE:
        aa_tree_walk_ordered( t, AA_LINK_RIGHT, wlker, accum );
        break;
    case  AA_WALK_ROOT_LEFT:
        aa_tree_walk_root( t, AA_LINK_LEFT, wlker, accum );
        break;
    case  AA_WALK_ROOT_RIGHT:
        aa_tree_walk_root( t, AA_LINK_RIGHT, wlker, accum );
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

int aa_tree_iterator_end( struct aa_tree_iterator *iter )
{
    return iter->current_ == NULL;
}

static void *void_ptr_copy(void *new_place, const void *element, size_t es )
{
    (void)(es);
    *((void **)new_place) = *((void **)element);
    return new_place;
}

static int aa_tree_iterator_shift( struct aa_tree_iterator *iter,
                                   struct aa_tree_node *root )
{
    while( root ) {
        if( !cnt_deque_push_front2( iter->stack_, &root, void_ptr_copy ) )
            return 0;
        root = root->links_[iter->push_side_];
    }
    if( !cnt_deque_empty( iter->stack_ ) ) {
        void **top = cnt_deque_front(iter->stack_);
        root = (struct aa_tree_node *)*top;
        cnt_deque_pop_front( iter->stack_ );
    }
    iter->current_ = root;
    return 1;
}


int aa_tree_iterator_next( struct aa_tree_iterator *iter )
{
    struct aa_tree_node *root = iter->current_->links_[iter->next_side_];
    return aa_tree_iterator_shift( iter, root );
}

static struct aa_tree_iterator
        *aa_tree_iterator_new_both( const struct aa_tree *aat, int forward)
{
    struct aa_tree_iterator *new_iter = (struct aa_tree_iterator *)
                        aa_tree_malloc( sizeof(struct aa_tree_iterator));
    int res = 0;
    if( new_iter ) {
        new_iter->stack_ =
                cnt_deque_new_reserved_pos( sizeof(void *),
                                            aat->root_->level_ << 1,
                                            DEQUE_START_BOTTOM);
        if( new_iter->stack_ ) {
            res = 1;
            new_iter->parent_     = aat;
            new_iter->current_    = aat->root_;
            new_iter->push_side_  = ( forward == 0 );
            new_iter->next_side_  = ( forward != 0 );

            if( !aa_tree_iterator_shift( new_iter, new_iter->current_ ) ) {
                cnt_deque_free( new_iter->stack_ );
                res = 0;
            }
        }
    }
    if( new_iter && !res ) {
        aa_tree_freemem(new_iter);
        new_iter = NULL;
    }
    return new_iter;
}

struct aa_tree_iterator *aa_tree_iterator_new( const struct aa_tree *aat )
{
    return aa_tree_iterator_new_both( aat, 1 );
}

struct aa_tree_iterator *aa_tree_reverse_iterator_new(const struct aa_tree *aat)
{
    return aa_tree_iterator_new_both( aat, 0 );
}

void aa_tree_iterator_free( struct aa_tree_iterator *iter )
{
    if( iter ) {
        cnt_deque_free(iter->stack_);
        aa_tree_freemem(iter);
    }
}

void *aa_tree_iterator_get( struct aa_tree_iterator *iter )
{
    return iter->current_->data_.ptr_;
}

static int aa_tree_copy_stack( struct cnt_deque *d, const struct cnt_deque *s )
{
    struct cnt_deque_iterator *itr = cnt_deque_reverse_iterator_new( s );
    int res = 0;
    if( itr ) {
        res = 1;
        while( res && !cnt_deque_iterator_end( itr ) ) {
            void **next = (void **)cnt_deque_iterator_get( itr );
            res = cnt_deque_push_front2( d, (void *)next, void_ptr_copy );
            cnt_deque_iterator_next(itr);
        }
        cnt_deque_iterator_free( itr );
    }
    return res;
}

struct aa_tree_iterator
        *aa_tree_iterator_clone(const struct aa_tree_iterator *iter)
{
    struct aa_tree_iterator *new_iter = (struct aa_tree_iterator *)
            aa_tree_malloc( sizeof(struct aa_tree_iterator));
    int res = 0;
    if( new_iter ) {
        new_iter->stack_ =
                cnt_deque_new_reserved_pos( sizeof(void *),
                                            iter->parent_->root_->level_ << 1,
                                            DEQUE_START_BOTTOM);
        if( new_iter->stack_ ) {
            res = 1;
            new_iter->parent_     = iter->parent_;
            new_iter->current_    = iter->current_;
            new_iter->push_side_  = iter->push_side_;
            new_iter->next_side_  = iter->next_side_;

            if( !aa_tree_copy_stack( new_iter->stack_, iter->stack_ ) ) {
                cnt_deque_free( new_iter->stack_ );
                res = 0;
            }
        }
    }
    if( new_iter && !res ) {
        aa_tree_freemem(new_iter);
        new_iter = NULL;
    }
    return new_iter;
}

/*
static void aa_tree_non_rec_node_walk_test( struct aa_tree *aat )
{
    struct aa_tree_node *root = aat->root_;
    if( root ) {
        struct cnt_deque *deq =
                cnt_deque_new_reserved( sizeof(struct aa_tree_node *), 32 );
        while( 1 ) {
            while( root ) {
                if( !cnt_deque_push_front( deq, &root ) )
                    return;
                root = root->links_[0];
            }
            if( cnt_deque_empty( deq ) )
                break;
            root = (struct aa_tree_node *)(*((void **)cnt_deque_front( deq )));
            cnt_deque_pop_front( deq );
            //printf( "walk value %u\n", root->data_.number_ );
            root = root->links_[1];
        }
    }
}
*/
