#if !defined( LISTWORK_20131029_H )
#define LISTWORK_20131029_H

#include <stdlib.h>
#include "inc/struct-fields.h"

struct linked_list_header {
    struct linked_list_header *flink;
};

typedef void (*linked_list_foreach_func)(struct linked_list_header *f );
typedef void (*linked_list_foreach2_func)(struct linked_list_header *f, void *d);

struct linked_list_header *linked_list_last(struct linked_list_header *lst);
size_t linked_list_length(const struct linked_list_header *lst);

#define linked_list_insert( element, new_ptr, list_field )                \
    (new_ptr)->list_field.flink = (element)->list_field.flink,            \
    (element)->list_field.flink = &(new_ptr)->list_field

#define linked_list_insert_list( src, oth, list_field )                   \
    linked_list_last(&(oth)->list_field)->flink = (src)->list_field.flink,\
    (src)->list_field.flink = &(oth)->list_field


void linked_list_foreach( struct linked_list_header *lst,
                          linked_list_foreach_func func );

#endif // LISTWORK_H
