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

#define bilinked_list_insert( src, new_l, forward )                           \
    (new_l)->links_[forward!=0] = (src)->links_[forward!=0],                  \
    (src)  ->links_[forward!=0] = (new_l),                                    \
    (new_l)->links_[forward==0] = (src)

#define bilinked_list_insert_by_field( src, new_l, list_field, forward )      \
    bilinked_list_insert( &(src)->list_field, &(new_l)->list_field, forward )

#define bilinked_list_remove( src )                                           \
    (src)->links_[0] ?                                                        \
    (src)->links_[0]->links_[1] = (src)->links_[1] : (NULL),                  \
    (src)->links_[1] ?                                                        \
    (src)->links_[1]->links_[0] = (src)->links_[0] : (NULL)


#define bilinked_list_remove_by_field( src, list_field )                      \
    bilinked_list_remove( &(src)->list_field )

#define bilinked_list_next( l, forward ) ((l)->links_[forward!=0])


size_t bilinked_list_length( struct bilinked_list_head *bll,
                             enum bilinked_list_direction direct );

#endif // BILINKEDLIST_20131101_H
