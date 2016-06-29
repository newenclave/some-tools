#ifndef TEMPL_DEQUE_H
#define TEMPL_DEQUE_H

#include <stdint.h>
#include <stdlib.h>

#include "lists/bilinked-list.h"
#include "inc/struct-fields.h"

#if defined( _MSC_VER )
  #define INLINE_KEYWORD
#else
  #define INLINE_KEYWORD inline
#endif


#define deque_define_custom( type, type_name, backet_size, allo_f, deallo_f ) \
\
typedef struct _##type_name##_deq_node { \
    struct bilinked_list_head head_;     \
    type dat_[backet_size];              \
} type_name##_deq_node;\
\
typedef struct _##type_name {       \
    type_name##_deq_node *front_;   \
    type_name##_deq_node *back_;    \
    type                 *begin_;   \
    type                 *end_;     \
    size_t                len_;     \
} type_name;\
\
static INLINE_KEYWORD \
int type_name##_init( type_name *d ) \
{\
    memset( d, 0, sizeof(*d) ); \
    d->front_ = d->back_ = (type_name##_deq_node *)(allo_f)(sizeof(*d->back_));\
    if( !d->front_ ) { \
        return 0; \
    }\
    d->front_->head_.links_[0] = d->front_->head_.links_[1] = NULL;\
    d->begin_ = d->end_ = &d->front_->dat_[backet_size >> 1];\
    d->len_ = 0;\
    return 1;\
}\
\
static INLINE_KEYWORD \
void type_name##_free( type_name *d ) \
{\
    while( d->front_ ) {\
        type_name##_deq_node *old = d->front_; \
        d->front_ = FIELD_ENTRY( d->front_->head_.links_[1], \
                                 type_name##_deq_node, head_ );\
        BILINKED_LIST_REMOVE( &old->head_ );\
        (deallo_f)(old);\
    }\
}\
\
static INLINE_KEYWORD \
type *type_name##_emplace_front( type_name *d ) \
{\
    if( d->begin_ == &d->front_->dat_[0] ) {\
        type_name##_deq_node *node = \
                (type_name##_deq_node *)(allo_f)(sizeof(*d->back_));\
        if(!node) {\
            return NULL;\
        }\
        node->head_.links_[0] = node->head_.links_[1] = NULL;\
        BILINKED_LIST_INSERT( &d->front_->head_, &node->head_, 0 );\
        d->begin_ = &node->dat_[backet_size];\
        d->front_ = node;\
    }\
    ++d->len_;\
    return --d->begin_;\
}\
\
static INLINE_KEYWORD \
int type_name##_push_front( type_name *d, type data )\
{\
    type *f = type_name##_emplace_front( d );\
    if( f ) {\
        *f = data;\
        return 1;\
    } else {\
        return 0;\
    }\
}\
\
static INLINE_KEYWORD \
type *type_name##_emplace_back( type_name *d ) \
{\
    if( d->end_ == &d->back_->dat_[backet_size] ) { \
        type_name##_deq_node *node = \
                    (type_name##_deq_node *)(allo_f)(sizeof(*d->back_));\
        if( !node ) {\
            return NULL;\
        }\
        node->head_.links_[0] = node->head_.links_[1] = NULL;\
        BILINKED_LIST_INSERT( &d->back_->head_, &node->head_, 1 );\
        d->end_ = &node->dat_[0];\
        d->back_ = node;\
    }\
    ++d->len_;\
    return d->end_++;\
}\
\
static INLINE_KEYWORD \
int type_name##_push_back( type_name *d, type data )\
{\
    type *b = type_name##_emplace_back( d );\
    if( b ) {\
        *b = data;\
        return 1;\
    } else {\
        return 0;\
    }\
}\
\
static INLINE_KEYWORD \
void type_name##_pop_front( type_name *d ) \
{\
    if( d->end_ != d->begin_ ) {\
        ++d->begin_;\
        if( d->begin_ == &d->front_->dat_[backet_size] ) {\
            type_name##_deq_node *old = d->front_;\
            d->front_ = FIELD_ENTRY( old->head_.links_[1]->links_,\
                                     type_name##_deq_node, head_ );\
            BILINKED_LIST_REMOVE( &old->head_ );\
            (deallo_f)(old);\
            d->begin_ = &d->front_->dat_[0];\
        }\
        --d->len_;\
    }\
}\
\
static INLINE_KEYWORD \
void type_name##_pop_back( type_name *d )\
{\
    if( d->end_ != d->begin_ ) {\
        --d->end_;\
        if( d->end_ == &d->back_->dat_[0] ) {\
            type_name##_deq_node *old = d->back_;\
            d->back_ = FIELD_ENTRY( old->head_.links_[0]->links_,\
                                    type_name##_deq_node, head_ );\
            BILINKED_LIST_REMOVE( &old->head_ );\
            (deallo_f)(old);\
            d->end_ = &d->back_->dat_[backet_size];\
        }\
        --d->len_;\
    }\
}\
\
static INLINE_KEYWORD \
type *type_name##_back( type_name *d ) \
{\
    return (d->end_ - 1);\
}\
\
static INLINE_KEYWORD \
type *type_name##_front( type_name *d )\
{\
    return d->begin_;\
}\
\
static const size_t type_name##_backet_size = backet_size


#define deque_define_custom_type( type, type_name )     \
    deque_define_custom( type, type_name, 64, malloc, free )


#define deque_define_type( type )                   \
    deque_define_custom_type( type, type##_deque_type )

#define deque_lenght( deq ) (deq).len_


#endif

