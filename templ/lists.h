#ifndef TEMPL_LISTS_H
#define TEMPL_LISTS_H

struct linked_list_head {
    struct linked_list_head *link_;
};

#define linked_list_insert( element, new_ptr )      \
    (new_ptr)->link_ =  (element)->link_,           \
    (element)->link_ =   new_ptr

#define linked_list_insert_list( src, oth  )        \
    linked_list_last(oth)->flink = &(src)->link_,   \
    &(src)->link_ =                 &(oth)

#define linked_list_insert_by_field( element, new_ptr, field )     \
    linked_list_insert( &(element)->field, &(new_ptr)->field )

#define linked_list_insert_list_by_field( src, oth, field )        \
    linked_list_insert_list( &(src)->field, &(oth)->field )

//////////////// Doubly linked ////////////////

struct dlinked_list_head {
    struct dlinked_list_head *links_[2];
};

#define dlinked_list_insert( src, new_l, forward )                             \
    (&(new_l)->links_[forward!=0] = &(src)->links_[forward!=0],                \
     &(src)  ->links_[forward!=0] = &(new_l),                                  \
     &(new_l)->links_[forward==0] = &(src))

#define dlinked_list_insert_by_field( src, new_l, field, forward )             \
    dlinked_list_insert( &(src)->field, &(new_l)->field, forward )

#define dlinked_list_remove( src )                                             \
    (&(src)->links_[0] ?                                                       \
     &(src)->links_[0]->links_[1] = &(src)->links_[1] : (NULL),                \
     &(src)->links_[1] ?                                                       \
     &(src)->links_[1]->links_[0] = &(src)->links_[0] : (NULL))

#define dlinked_list_remove_by_field( src, field )                             \
    dlinked_list_remove(&(src)->field)

#define dlinked_list_step( l, forward ) ((l)->links_[forward!=0])

#define dlinked_list_next( l ) dlinked_list_step( l, 0 )

#define dlinked_list_prev( l ) dlinked_list_step( l, 1 )



#endif // LISTS_H
