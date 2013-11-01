#if !defined( LISTWORK_20131029_H )
#define LISTWORK_20131029_H

#include <stdlib.h>
#include "inc/struct-fields.h"

struct linked_list_head {
    struct linked_list_head *flink;
};

typedef int (*linked_list_map_func)(struct linked_list_head *f );
typedef int (*linked_list_map2_func)(struct linked_list_head *f, void *d);

struct linked_list_head *linked_list_last(struct linked_list_head *lst);
size_t linked_list_length(const struct linked_list_head *lst);


#define linked_list_insert( element, new_ptr )      \
    (new_ptr)->flink =  (element)->flink,           \
    (element)->flink =   new_ptr

#define linked_list_insert_list( src, oth  )        \
    linked_list_last(oth)->flink = (src)->flink,    \
    (src)->flink =                 (oth)

#define linked_list_insert_by_field( element, new_ptr, field_name )     \
    linked_list_insert( &(element)->field_name, &(new_ptr)->field_name )

#define linked_list_insert_list_by_field( src, oth, field_name )        \
    linked_list_insert_list( &(src)->field_name, &(oth)->field_name )

size_t linked_list_map( struct linked_list_head *lst,
                        linked_list_map_func func );

size_t linked_list_map2( struct linked_list_head *lst,
                         linked_list_map2_func func, void *user_data );

#endif // LISTWORK_H
