#ifndef LISTS_H
#define LISTS_H

#define list_define_custom_type( type, type_name )      \
  typedef struct type##_list_type {                     \
      struct type##_list_type  *nxt_;                   \
      type                      dat_;                   \
  } type_name

#define list_define_type( type )            \
    list_define_custom_type( type, type##_list_type )

#define list_insert_value( lst, value, result )     \
    do {                                            \
        void *next__ = malloc(sizeof(lst));         \
        if( next__ ) {                              \
            void *tmp__ = (lst).nxt_;               \
            (lst).nxt_       = next__;              \
            (lst).nxt_->dat_ = value;               \
            (lst).nxt_->nxt_ = tmp__;               \
        }                                           \
        result = next__;                            \
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

#endif // LISTS_H
