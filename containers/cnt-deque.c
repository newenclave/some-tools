
#include "../memory/mm-array.h"
#include "../lists/linked-list.h"


struct cnt_deque {
    struct linked_list_head list_;
};


struct cnt_deque* cnt_deque_new( )
{
    struct cnt_deque *new_deq =
            (struct cnt_deque *)calloc(1, sizeof(struct cnt_deque));

    if( new_deq ) {

    }

    return new_deq;
}

void cnt_deque_free( struct cnt_deque *cnd )
{
    if( cnd ) {
        free(cnd);
    }
}
