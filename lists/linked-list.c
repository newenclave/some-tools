
#include "linked-list.h"

struct linked_list_header *linked_list_last(struct linked_list_header *lst)
{
    while( 0 != lst->flink ) lst = lst->flink;
    return lst;
}

size_t linked_list_length(const struct linked_list_header *lst)
{
    size_t len = 0;
    for( ;lst; lst = lst->flink, ++len);
    return len;
}


size_t linked_list_map( struct linked_list_header *lst,
                        linked_list_map_func func )
{
    size_t complete = 0;
    while( lst ) {
        struct linked_list_header * next = lst->flink;

        if( 0 == func( lst ) )
            break;

        ++complete;
        lst = next;
    }
    return complete;
}

size_t linked_list_map2( struct linked_list_header *lst,
                         linked_list_map2_func func, void *user_data )
{
    size_t complete = 0;
    while( lst ) {
        struct linked_list_header * next = lst->flink;

        if( 0 == func( lst, user_data ) )
            break;

        ++complete;
        lst = next;
    }
    return complete;
}
