#ifndef TEMPL_LISTS_H
#define TEMPL_LISTS_H

#define list_define_custom_type( type, type_name )      \
  typedef struct type##_list_type {                     \
      struct type##_list_type  *nxt_;                   \
      type                      dat_;                   \
  } type_name

#define list_define_type( type )                        \
    list_define_custom_type( type, type##_list_type )

#define list_insert_value( lst, value, result )         \
    do {                                                \
        void *next__ = malloc(sizeof(lst));             \
        if( next__ ) {                                  \
            void *tmp__ = (lst).nxt_;                   \
            (lst).nxt_       = next__;                  \
            (lst).nxt_->dat_ = value;                   \
            (lst).nxt_->nxt_ = tmp__;                   \
        }                                               \
        result = next__;                                \
    } while(0)

#define list_delete_next( lst )             \
    do {                                    \
        void *next__ = (lst).nxt_;          \
        (lst).nxt_ = (lst).nxt_->nxt_;      \
        free(next__);                       \
    } while(0)

#define list_free(lst)                      \
    do {                                    \
        while( (lst).nxt_ ) {               \
            list_delete_next(lst);          \
        }                                   \
    } while( 0 )

#define list_foreach( lst, node ) for( node = &(lst); node; node = node->nxt_ )

#define list_init( default_value )    { NULL, default_value }

//////////////// Doubly linked ////////////////

struct dlinked_list_head {
    struct dlinked_list_head *links_[2];
};

#define dlinked_list_insert( src, new_l, forward )                             \
    (&(new_l)->links_[forward!=0] = &(src)->links_[forward!=0],                \
     &(src)  ->links_[forward!=0] = &(new_l),                                  \
     &(new_l)->links_[forward==0] = &(src))

#define dlinked_list_insert_by_field( src, new_l, field, forward )             \
    dlinked_list_insert( &(src)->field, &(new_l)->field, forward )

#define dlinked_list_remove( src )                                             \
    (&(src)->links_[0] ?                                                       \
     &(src)->links_[0]->links_[1] = &(src)->links_[1] : (NULL),                \
     &(src)->links_[1] ?                                                       \
     &(src)->links_[1]->links_[0] = &(src)->links_[0] : (NULL))

#define dlinked_list_remove_by_field( src, field )                             \
    dlinked_list_remove(&(src)->field)

#define dlinked_list_step( l, forward ) ((l)->links_[forward!=0])

#define dlinked_list_next( l ) dlinked_list_step( l, 0 )

#define dlinked_list_prev( l ) dlinked_list_step( l, 1 )



#endif // LISTS_H
