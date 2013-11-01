#ifndef CNT_DEQUE_20131111_H
#define CNT_DEQUE_20131111_H

#include <stdlib.h>

struct cnt_deque;

struct cnt_deque* cnt_deque_new( );
void              cnt_deque_free( struct cnt_deque *cnd );

#endif // CNT_DEQUE_20131111_H
