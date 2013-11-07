#ifndef BILINKEDLIST_20131101_H
#define BILINKEDLIST_20131101_H

#include <stdlib.h>

struct bilinked_list_head {
    struct bilinked_list_head *links_[2];
};

enum bilinked_list_direction  {
     BILINK_DIRECT_BACKWARD  = 0
    ,BILINK_DIRECT_FORWARD   = 1
};

#define BILINKED_LIST_INSERT( src, new_l, forward )                           \
    ((new_l)->links_[forward!=0] = (src)->links_[forward!=0],                 \
     (src)  ->links_[forward!=0] = (new_l),                                   \
     (new_l)->links_[forward==0] = (src))

#define BILINKED_LIST_INSERT_BY_FIELD( src, new_l, list_field, forward )      \
    BILINKED_LIST_INSERT( &(src)->list_field, &(new_l)->list_field, forward )

#define BILINKED_LIST_REMOVE( src )                                           \
    ((src)->links_[0] ?                                                       \
     (src)->links_[0]->links_[1] = (src)->links_[1] : (NULL),                 \
     (src)->links_[1] ?                                                       \
     (src)->links_[1]->links_[0] = (src)->links_[0] : (NULL))


#define BILINKED_LIST_REMOVE_BY_FIELD( src, list_field )                      \
    BILINKED_LIST_REMOVE(&(src)->list_field)


#define BILINKED_LIST_STEP( l, forward ) ((l)->links_[forward!=0])

#define BILINKED_LIST_NEXT( l ) \
    BILINKED_LIST_STEP( l, BILINK_DIRECT_FORWARD )

#define BILINKED_LIST_PREV( l ) \
    BILINKED_LIST_STEP( l, BILINK_DIRECT_BACKWARD )


size_t bilinked_list_length( struct bilinked_list_head *bll,
                             enum bilinked_list_direction direct );

#endif // BILINKEDLIST_20131101_H
