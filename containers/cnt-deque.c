
#include <memory.h>

#include "cnt-deque.h"
#include "../lists/bilinked-list.h"
#include "../inc/struct-fields.h"

struct cnt_deque_unit {
    struct bilinked_list_head list_;
    void                     *array_;
    size_t                    length_;
};

enum cnt_deque_direction {
     SIDE_FRONT = 0
    ,SIDE_BACK  = 1
};

struct cnt_deque {
    struct cnt_deque_unit  *units_[2];
    void                   *sides_[2];
    size_t                  element_size_;
    size_t                  count_;
    cnt_deque_element_free  free_;
};

struct cnt_deque_iterator
{
    struct cnt_deque       *parent_;
    struct cnt_deque_unit  *unit_;
    short  next_;
    short  prev_;
};


#define CNT_DEQUE_ELEMENT_NEXT( cnd, ptr ) \
            (((char *)ptr) + ((cnd)->element_size_))

#define CNT_DEQUE_ELEMENT_PREV( cnd, ptr ) \
            (((char *)ptr) - ((cnd)->element_size_))

#define CNT_DEQUE_BLOCK_BEGIN( unit ) ((unit)->array_)
#define CNT_DEQUE_BLOCK_AT( unit, element_size, count ) \
    (((unit)->array_ + ((element_size) * count)))

#define CNT_DEQUE_BLOCK_END( unit ) ((unit)->array_ + (unit)->length_)

#define CNT_DEQUE_BLOCK_SIDE( unit, side )        \
        ((side) ? CNT_DEQUE_BLOCK_BEGIN(unit)     \
                : CNT_DEQUE_BLOCK_END(unit) )

#define CNT_DEQUE_BLOCK_IS_BEGIN( unit, ptr ) \
        (CNT_DEQUE_BLOCK_BEGIN( unit ) == (ptr))

#define CNT_DEQUE_BLOCK_IS_END( unit, ptr ) \
        (CNT_DEQUE_BLOCK_END( unit ) == (ptr))

#define CNT_DEQUE_BLOCK_IS_SIDE( unit, ptr, side )      \
        ((side) ? CNT_DEQUE_BLOCK_IS_BEGIN( unit, ptr )   \
                : CNT_DEQUE_BLOCK_IS_END( unit, ptr ) )

#define CNT_DEQUE_IS_TOP( cnd )          \
        ((cnd)->sides_[SIDE_FRONT] == (cnd)->units_[SIDE_FRONT]->array_ )

#define CNT_DEQUE_IS_BOTTOM( cnd )                                             \
        ((cnd)->sides_[SIDE_BACK] == ((cnd)->units_[SIDE_BACK]->array_ \
                        + (cnd)->units_[SIDE_BACK]->length_))

#define CNT_DEQUE_DEF_INC(size) ((size) + ((size) >> 1))

struct cnt_deque_unit *cnt_deque_unit_create( struct cnt_deque* cnd,
                                              size_t elements )
{
    struct cnt_deque_unit *new_unit =
            (struct cnt_deque_unit *)calloc(1, sizeof(struct cnt_deque_unit) );
    if( new_unit ) {
        new_unit->length_ = elements * cnd->element_size_;
        new_unit->array_ = (char *)malloc( new_unit->length_ );
        if( !new_unit->array_ ) {
            free( new_unit );
            new_unit = NULL;
        }
    }
    return new_unit;
}

void cnt_deque_init_unit_position( struct cnt_deque *cnd, size_t reserve,
                                   enum cnt_deque_start_point position)
{
    switch( position ) {
    case DEQUE_START_TOP:
        cnd->sides_[SIDE_FRONT] = cnd->sides_[SIDE_BACK] =
                CNT_DEQUE_BLOCK_BEGIN( cnd->units_[SIDE_FRONT] );
        break;
    case DEQUE_START_MIDDLE:
        cnd->sides_[SIDE_FRONT] = cnd->sides_[SIDE_BACK] =
                CNT_DEQUE_BLOCK_AT( cnd->units_[SIDE_FRONT], cnd->element_size_,
                                    reserve >> 1);
        break;
    case DEQUE_START_BOTTOM:
        cnd->sides_[SIDE_FRONT] = cnd->sides_[SIDE_BACK] =
                CNT_DEQUE_BLOCK_END( cnd->units_[SIDE_FRONT] );
        break;
    }
}

struct cnt_deque* cnt_deque_new_all( size_t element_size,
                                     size_t init_reserve,
                                     cnt_deque_element_free free_call,
                                     enum cnt_deque_start_point position)
{
    struct cnt_deque *new_deq =
            (struct cnt_deque *)malloc(sizeof(struct cnt_deque));
    if( new_deq ) {
        memset( new_deq, 0, sizeof(struct cnt_deque) );
        new_deq->element_size_ = element_size;
        new_deq->free_         = free_call;
        struct cnt_deque_unit *unit =
                cnt_deque_unit_create( new_deq,
                                       init_reserve ? init_reserve : 8);
        if( unit ) {
            new_deq->units_[SIDE_FRONT] = new_deq->units_[SIDE_BACK] = unit;
            cnt_deque_init_unit_position( new_deq, init_reserve, position );
        } else {
            free(new_deq);
            new_deq = NULL;
        }
    }
    return new_deq;
}

struct cnt_deque* cnt_deque_new_reserved2( size_t element_size,
                                           size_t init_reserve,
                                           cnt_deque_element_free free_call)
{
    return cnt_deque_new_all( element_size, init_reserve,
                              free_call, DEQUE_START_MIDDLE );
}

struct cnt_deque* cnt_deque_new_reserved( size_t element_size,
                                           size_t init_reserve)
{
    return cnt_deque_new_all( element_size, init_reserve,
                              NULL, DEQUE_START_MIDDLE );
}

struct cnt_deque *cnt_deque_new_reserved_pos ( size_t element_size,
                                     size_t init_reserve,
                                     enum cnt_deque_start_point position)
{
    return cnt_deque_new_all( element_size, init_reserve, NULL, position );
}

struct cnt_deque* cnt_deque_new2( size_t element_size,
                                  cnt_deque_element_free free_call )
{
    return cnt_deque_new_all( element_size, 8, free_call, DEQUE_START_MIDDLE );
}

struct cnt_deque* cnt_deque_new( size_t element_size )
{
    return cnt_deque_new_all( element_size, 8, NULL, DEQUE_START_MIDDLE );
}

void  cnt_deque_set_free( struct cnt_deque *cnd,
                                      cnt_deque_element_free free_call )
{
    cnd->free_ = free_call;
}


void *cnt_deque_front( struct cnt_deque *cnd )
{
    return cnd->sides_[SIDE_FRONT];
}

void *cnt_deque_back( struct cnt_deque *cnd )
{
    void *ptr = NULL;
    ptr = CNT_DEQUE_ELEMENT_PREV( cnd, cnd->sides_[SIDE_BACK] );
    return ptr;
}

int cnt_deque_create_side( struct cnt_deque *cnd, int side )
{

    struct cnt_deque_unit *new_side = NULL;

    side = (side != SIDE_FRONT);

    new_side = FIELD_ENTRY(
                    BILINKED_LIST_STEP( &cnd->units_[side]->list_, side ),
                        struct cnt_deque_unit, list_);

    if( !new_side ) {
        const size_t old_size = cnd->units_[side]->length_ / cnd->element_size_;
        const size_t new_size = CNT_DEQUE_DEF_INC(old_size);
        new_side = cnt_deque_unit_create( cnd, new_size );
    }

    if( new_side ) {
        BILINKED_LIST_INSERT( &cnd->units_[side]->list_,
                              &new_side->list_, side );
        cnd->units_[side] = new_side;
        cnd->sides_[side] = CNT_DEQUE_BLOCK_SIDE( new_side, side );
    }

    return new_side != NULL;
}

void *cnt_deque_create_front( struct cnt_deque *cnd )
{
    void *new_front_ptr = NULL;
    int res = 1;
    if( CNT_DEQUE_IS_TOP( cnd ) ) {
        res = cnt_deque_create_side( cnd, SIDE_FRONT );
    }
    if( res ) {
        cnd->sides_[SIDE_FRONT] =
                CNT_DEQUE_ELEMENT_PREV( cnd, cnd->sides_[SIDE_FRONT] );
        new_front_ptr = cnd->sides_[SIDE_FRONT];
        ++cnd->count_;
    }
    return new_front_ptr;
}

int cnt_deque_push_front2(struct cnt_deque *cnd, void *element,
                          cnt_deque_element_copy copy_call)
{
    void *new_front = cnt_deque_create_front( cnd );
    if( new_front && copy_call) {
        copy_call( new_front, element, cnd->element_size_ );
    }
    return new_front != NULL;
}

void *cnt_deque_memcopy( void *new_place, void *element, size_t element_size )
{
    return memcpy( new_place, element, element_size );
}

int cnt_deque_push_front( struct cnt_deque *cnd, void *element )
{
    return cnt_deque_push_front2( cnd, element, cnt_deque_memcopy );
}

void cnt_deque_pop_side( struct cnt_deque *cnd, int side,
                        cnt_deque_element_free free_call )
{
    void *old_side = NULL;
    void *new_side = NULL;

    side = (side != SIDE_FRONT);

    old_side = cnd->sides_[side];
    new_side = side ? CNT_DEQUE_ELEMENT_PREV( cnd, old_side )
                    : CNT_DEQUE_ELEMENT_NEXT( cnd, old_side );

    if( free_call ) {
        free_call( old_side );
    }

    if( CNT_DEQUE_BLOCK_IS_SIDE(cnd->units_[side], new_side, side)
            && new_side!=cnd->sides_[!side] )
    {
        struct bilinked_list_head  *next_unit =
                BILINKED_LIST_STEP(&cnd->units_[side]->list_, !side);
        cnd->units_[side] =
                FIELD_ENTRY(next_unit, struct cnt_deque_unit, list_);
        new_side = CNT_DEQUE_BLOCK_SIDE(cnd->units_[side], !side);
    }

    cnd->sides_[side] = new_side;
    --cnd->count_;
}

int cnt_deque_pop_front2( struct cnt_deque *cnd,
                          cnt_deque_element_free free_call)
{
    cnt_deque_pop_side( cnd, SIDE_FRONT, free_call );
    return 1;
}

int cnt_deque_pop_front ( struct cnt_deque *cnd )
{
    return cnt_deque_pop_front2( cnd, cnd->free_ );
}

int cnt_deque_pop_back2 ( struct cnt_deque *cnd,
                          cnt_deque_element_free free_call )
{
    cnt_deque_pop_side( cnd, SIDE_BACK, free_call );
    return 1;
}

int cnt_deque_pop_back ( struct cnt_deque *cnd )
{
    return cnt_deque_pop_back2( cnd, cnd->free_ );
}

void *cnt_deque_create_back( struct cnt_deque *cnd)
{
    void *new_back = NULL;
    int res = 1;
    if( CNT_DEQUE_IS_BOTTOM( cnd ) ) {
        res = cnt_deque_create_side( cnd, SIDE_BACK );
    }
    if( res ) {
        new_back = cnd->sides_[SIDE_BACK];
        cnd->sides_[SIDE_BACK] =
                CNT_DEQUE_ELEMENT_NEXT( cnd, cnd->sides_[SIDE_BACK] );
        ++cnd->count_;
    }
    return new_back;
}


int cnt_deque_push_back2( struct cnt_deque *cnd, void *element,
                          cnt_deque_element_copy copy_call )
{
    void *new_back = cnt_deque_create_back( cnd );
    if( new_back && copy_call )
        copy_call( new_back, element, cnd->element_size_ );
    return new_back != NULL;
}

int cnt_deque_push_back ( struct cnt_deque *cnd, void *element )
{
    return cnt_deque_push_back2( cnd, element, NULL );
}

int cnt_deque_empty( struct cnt_deque *cnd )
{
    //return CNT_DEQUE_EMPTY_LOCAL(cnd);
    return cnd->sides_[SIDE_FRONT] == cnd->sides_[SIDE_BACK];

}

size_t cnt_deque_size ( struct cnt_deque *cnd )
{
    return cnd->count_;
}

int cnt_deque_unit_free( struct cnt_deque *cnd,
                         struct cnt_deque_unit  *unit,
                         void *begin, const void *end,
                         cnt_deque_element_free free_call )
{
    if( unit ) {
        void *block_end = ((char *)unit->array_) + unit->length_;
        size_t element_size = cnd->element_size_;
        while( begin != end && begin != block_end ) {
            free_call( begin );
            begin = ((char *)begin) + element_size;
        }
        return (begin != end);
    }
    return 0;
}

void cnt_deque_list_free( struct cnt_deque *cnd,
                          cnt_deque_element_free free_call )
{
    // free elements
    struct cnt_deque_unit     *unit = NULL;
    struct bilinked_list_head *head = &cnd->units_[SIDE_FRONT]->list_;

    if( free_call ) {
        void *begin     = NULL;
        const void *end = cnd->sides_[SIDE_BACK];
        while( head ) {
            unit = FIELD_ENTRY( head, struct cnt_deque_unit, list_ );
            begin = begin ? unit->array_
                          : cnd->sides_[SIDE_FRONT];
            head = BILINKED_LIST_NEXT( head );
            if(!cnt_deque_unit_free( cnd, unit, begin, end, free_call )) {
                head = NULL;
            }
        }
    }
    // free units
    head = &cnd->units_[SIDE_FRONT]->list_;
    while( head->links_[0] ) head = head->links_[0];
    while( head ) {
        unit = FIELD_ENTRY( head, struct cnt_deque_unit, list_ );
        head = BILINKED_LIST_NEXT( head );
        free( unit->array_ );
        free( unit );
    }
}

void cnt_deque_free2( struct cnt_deque *cnd, cnt_deque_element_free free_call )
{
    if( cnd ) {
        cnt_deque_list_free( cnd, free_call );
        free(cnd);
    }
}

void cnt_deque_free( struct cnt_deque *cnd )
{
    cnt_deque_free2( cnd, cnd->free_ );
}

