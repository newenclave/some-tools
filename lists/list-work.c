
#include "list-work.h"

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


void linked_list_foreach( struct linked_list_header *lst,
                          linked_list_foreach_func func )
{
    while( lst ) {
        struct linked_list_header * next = lst->flink;
        func( lst );
        lst = next;
    }
}

