#ifndef CNT_DEQUE_20131111_H
#define CNT_DEQUE_20131111_H

#include <stdlib.h>

struct cnt_deque;

typedef void  (* cnt_deque_element_free)( void *element );
typedef void *(* cnt_deque_element_copy)( void *new_place,
                                          void *element,
                                          size_t element_size );

enum cnt_deque_start_point {
     DEQUE_START_TOP    = 0
    ,DEQUE_START_MIDDLE = 1
    ,DEQUE_START_BOTTOM = 2
};

struct cnt_deque *cnt_deque_new( size_t element_size  );
struct cnt_deque *cnt_deque_new2( size_t element_size,
                                  cnt_deque_element_free free_call );
struct cnt_deque *cnt_deque_new_reserved ( size_t element_size,
                                           size_t init_reserve);
struct cnt_deque *cnt_deque_new_reserved2( size_t element_size,
                                           size_t init_reserve,
                                           cnt_deque_element_free free_call);

struct cnt_deque *cnt_deque_new_reserved_pos ( size_t element_size,
                                     size_t init_reserve,
                                     enum cnt_deque_start_point position);

void              cnt_deque_set_free( struct cnt_deque *cnd,
                                      cnt_deque_element_free free_call );

void              cnt_deque_free( struct cnt_deque *cnd );
void              cnt_deque_free2( struct cnt_deque *cnd,
                                   cnt_deque_element_free free_call );

int               cnt_deque_empty( struct cnt_deque *cnd );
size_t            cnt_deque_size ( struct cnt_deque *cnd );
void             *cnt_deque_front( struct cnt_deque *cnd );
void             *cnt_deque_back ( struct cnt_deque *cnd );

int               cnt_deque_pop_front ( struct cnt_deque *cnd );
int               cnt_deque_pop_front2( struct cnt_deque *cnd,
                                        cnt_deque_element_free free_call);
void             *cnt_deque_create_front( struct cnt_deque *cnd);
int               cnt_deque_push_front( struct cnt_deque *cnd, void *element );
int               cnt_deque_push_front2(struct cnt_deque *cnd, void *element,
                                        cnt_deque_element_copy copy_call);

int               cnt_deque_pop_back  ( struct cnt_deque *cnd );
int               cnt_deque_pop_back2 ( struct cnt_deque *cnd,
                                        cnt_deque_element_free free_call );

void             *cnt_deque_create_back( struct cnt_deque *cnd);
int               cnt_deque_push_back  ( struct cnt_deque *cnd, void *element );
int               cnt_deque_push_back2 ( struct cnt_deque *cnd, void *element,
                                        cnt_deque_element_copy copy_call);

#endif // CNT_DEQUE_20131111_H
