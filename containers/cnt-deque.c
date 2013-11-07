
#include <memory.h>

#include "cnt-deque.h"
#include "../lists/bilinked-list.h"
#include "../inc/struct-fields.h"

struct cnt_deque_unit {
    struct bilinked_list_head list_;
    char                     *array_;
    size_t                    length_;
};

enum cnt_deque_direction {
     DIRECT_TOP     = 0
    ,DIRECT_BOTTOM  = 1
};

struct cnt_deque {
    struct cnt_deque_unit  *units_[2];
    void                   *sides_[2];
    size_t                  element_size_;
    size_t                  count_;
    cnt_deque_element_free  free_;
};

#define CNT_DEQUE_ELEMENT_NEXT( cnd, ptr ) \
            (((char *)ptr) + ((cnd)->element_size_))

#define CNT_DEQUE_ELEMENT_PREV( cnd, ptr ) \
            (((char *)ptr) - ((cnd)->element_size_))

#define CNT_DEQUE_BLOCK_BEGIN( unit ) ((unit)->array_)
#define CNT_DEQUE_BLOCK_AT( unit, element_size, count ) \
    (((unit)->array_ + ((element_size) * count)))

#define CNT_DEQUE_BLOCK_END( unit ) ((unit)->array_ + (unit)->length_)

#define cnt_deque_block_is_end( unit, ptr ) \
        (CNT_DEQUE_BLOCK_END( unit ) == (ptr))

#define CNT_DEQUE_IS_TOP( cnd )          \
        ((cnd)->sides_[DIRECT_TOP] == (cnd)->units_[DIRECT_TOP]->array_ )

#define CNT_DEQUE_IS_BOTTOM( cnd )                                             \
        ((cnd)->sides_[DIRECT_BOTTOM] == ((cnd)->units_[DIRECT_BOTTOM]->array_ \
                        + (cnd)->units_[DIRECT_BOTTOM]->length_))

#define CNT_DEQUE_EMPTY_LOCAL( cnd ) \
    ((cnd)->sides_[DIRECT_TOP] == (cnd)->sides_[DIRECT_BOTTOM])

#define CNT_DEQUE_DEF_INC(size) (size + (size >> 1))

#define CNT_DEQUE_PREF_TOP_SIZE( cnd )                              \
    cnt_deque_calc_pref_size((cnd)->units_[DIRECT_TOP]->length_,    \
                             (cnd)->element_size_)

#define CNT_DEQUE_PREF_BOT_SIZE( cnd )                              \
    cnt_deque_calc_pref_size((cnd)->units_[DIRECT_BOTTOM]->length_, \
                             (cnd)->element_size_)

size_t cnt_deque_calc_pref_size( size_t old_size, size_t element_size )
{
    size_t new_count = CNT_DEQUE_DEF_INC(old_size / element_size);
    return new_count;
}


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

void cnt_deque_unit_free_no_arr( struct cnt_deque_unit  *unit )
{
    free( unit->array_ );
    free( unit );
}

void cnt_deque_unit_free( struct cnt_deque *cnd,
                          struct cnt_deque_unit  *unit,
                          void *begin, void *end,
                          cnt_deque_element_free free_call )
{
    void *arr_end = CNT_DEQUE_BLOCK_END( unit );

    while ( begin != end && begin != arr_end ) {
        if( free_call ) free_call( begin );
        begin = CNT_DEQUE_ELEMENT_NEXT( cnd, begin );
    }
    cnt_deque_unit_free_no_arr( unit );
}

void cnt_deque_init_unit_position( struct cnt_deque *cnd, size_t reserve,
                                   enum cnt_deque_start_point position)
{
    switch( position ) {
    case DEQUE_START_TOP:
        cnd->sides_[DIRECT_TOP] = cnd->sides_[DIRECT_BOTTOM] =
                CNT_DEQUE_BLOCK_BEGIN( cnd->units_[DIRECT_TOP] );
        break;
    case DEQUE_START_MIDDLE:
        cnd->sides_[DIRECT_TOP] = cnd->sides_[DIRECT_BOTTOM] =
                CNT_DEQUE_BLOCK_AT( cnd->units_[DIRECT_TOP], cnd->element_size_,
                                    reserve >> 1);
        break;
    case DEQUE_START_BOTTOM:
        cnd->sides_[DIRECT_TOP] = cnd->sides_[DIRECT_BOTTOM] =
                CNT_DEQUE_BLOCK_END( cnd->units_[DIRECT_TOP] );
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
            new_deq->units_[DIRECT_TOP] = new_deq->units_[DIRECT_BOTTOM] = unit;
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
    return CNT_DEQUE_EMPTY_LOCAL(cnd) ? NULL : cnd->sides_[DIRECT_TOP];
}

void *cnt_deque_back( struct cnt_deque *cnd )
{
    void *ptr = NULL;
    if( !CNT_DEQUE_EMPTY_LOCAL(cnd) ) {
        ptr = CNT_DEQUE_ELEMENT_PREV( cnd, cnd->sides_[DIRECT_BOTTOM] );
    }
    return ptr;
}

void *cnt_deque_create_front( struct cnt_deque *cnd )
{
    void *new_front_ptr = NULL;
    int res = 0;
    if( CNT_DEQUE_IS_TOP( cnd ) ) {
        struct cnt_deque_unit *new_top =
                cnt_deque_unit_create( cnd, CNT_DEQUE_PREF_TOP_SIZE(cnd) );

        if( new_top ) {
            BILINKED_LIST_INSERT( &cnd->units_[DIRECT_TOP]->list_,
                                  &new_top->list_, BILINK_DIRECT_BACKWARD );
            cnd->units_[DIRECT_TOP] = new_top;
            cnd->sides_[DIRECT_TOP] = CNT_DEQUE_BLOCK_END( new_top );
            res = 1;
        }
    } else {
        res  = 1;
    }
    if( res ) {
        cnd->sides_[DIRECT_TOP] =
                CNT_DEQUE_ELEMENT_PREV( cnd, cnd->sides_[DIRECT_TOP] );
        new_front_ptr = cnd->sides_[DIRECT_TOP];
        ++cnd->count_;
    }
    return new_front_ptr;
}



int cnt_deque_push_front2(struct cnt_deque *cnd, void *element,
                          cnt_deque_element_copy copy_call)
{
    void *new_front = cnt_deque_create_front( cnd );
    printf( "push front\n" );
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

int cnt_deque_pop_front2( struct cnt_deque *cnd,
                          cnt_deque_element_free free_call)
{
    int res = 0;
    if( !CNT_DEQUE_EMPTY_LOCAL(cnd) ) {
        res = 1;
        void *old_top = cnd->sides_[DIRECT_TOP];
        void *new_top = CNT_DEQUE_ELEMENT_NEXT( cnd, old_top );
        if( free_call ) {
            free_call( old_top );
        }
        if( cnt_deque_block_is_end(cnd->units_[DIRECT_TOP], new_top)
                                && new_top!=cnd->sides_[DIRECT_BOTTOM])
        { 
            struct bilinked_list_head  *next_unit =
                    BILINKED_LIST_NEXT(&cnd->units_[DIRECT_TOP]->list_);
            BILINKED_LIST_REMOVE( &cnd->units_[DIRECT_TOP]->list_ );
            cnt_deque_unit_free_no_arr( cnd->units_[DIRECT_TOP] );
            cnd->units_[DIRECT_TOP] =
                    FIELD_ENTRY(next_unit, struct cnt_deque_unit, list_);
            new_top = cnd->units_[DIRECT_TOP]->array_;
        }
        cnd->sides_[DIRECT_TOP] = new_top;
        --cnd->count_;
    }
    return res;
}

int cnt_deque_pop_front ( struct cnt_deque *cnd )
{
    return cnt_deque_pop_front2( cnd, cnd->free_ );
}



int cnt_deque_pop_back2 ( struct cnt_deque *cnd,
                          cnt_deque_element_free free_call )
{
    int res = 0;
    if( !CNT_DEQUE_EMPTY_LOCAL(cnd) ) {
        res = 1;
        void *new_last =
                CNT_DEQUE_ELEMENT_PREV(cnd, cnd->sides_[DIRECT_BOTTOM]);
        if( free_call ) {
            free_call( new_last );
        }
        char *last_arr = cnd->units_[DIRECT_BOTTOM]->array_;
        if( (last_arr == new_last) && new_last!=cnd->sides_[DIRECT_TOP]) {
            struct bilinked_list_head  *next_unit =
                    BILINKED_LIST_NEXT(&cnd->units_[DIRECT_BOTTOM]->list_);

            BILINKED_LIST_REMOVE( &cnd->units_[DIRECT_BOTTOM]->list_ );
            cnt_deque_unit_free_no_arr( cnd->units_[DIRECT_BOTTOM] );

            cnd->units_[DIRECT_BOTTOM] =
                    FIELD_ENTRY(next_unit, struct cnt_deque_unit, list_);
            new_last = cnd->units_[DIRECT_BOTTOM]->array_;
        }
        cnd->sides_[DIRECT_BOTTOM] = new_last;
        --cnd->count_;
    }
    return res;
}

int cnt_deque_pop_back ( struct cnt_deque *cnd )
{
    return cnt_deque_pop_back2( cnd, cnd->free_ );
}

void *cnt_deque_create_back( struct cnt_deque *cnd)
{
    void *new_back = NULL;
    int res = 0;
    if( CNT_DEQUE_IS_BOTTOM( cnd ) ) {
        struct cnt_deque_unit *new_bottom =
                cnt_deque_unit_create( cnd, CNT_DEQUE_PREF_BOT_SIZE(cnd) );

        if( new_bottom ) {
            BILINKED_LIST_INSERT( &cnd->units_[DIRECT_BOTTOM]->list_,
                                  &new_bottom->list_, BILINK_DIRECT_FORWARD );
            cnd->units_[DIRECT_BOTTOM] = new_bottom;
            cnd->sides_[DIRECT_BOTTOM] = new_bottom->array_;
            res = 1;
        }
    } else {
        res = 1;
    }
    if( res ) {
        new_back = cnd->sides_[DIRECT_BOTTOM];
        cnd->sides_[DIRECT_BOTTOM] =
                CNT_DEQUE_ELEMENT_NEXT( cnd, cnd->sides_[DIRECT_BOTTOM] );
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
    return CNT_DEQUE_EMPTY_LOCAL(cnd);
}

size_t cnt_deque_size ( struct cnt_deque *cnd )
{
    return cnd->count_;
}

void cnt_deque_list_free( struct cnt_deque *cnd,
                          struct cnt_deque_unit  *unit,
                          cnt_deque_element_free free_call )
{
    void *begin = cnd->sides_[DIRECT_TOP];
    void *end   = cnd->sides_[DIRECT_BOTTOM];
    while( unit ) {
        struct bilinked_list_head *lh = BILINKED_LIST_NEXT( &unit->list_);
        cnt_deque_unit_free( cnd, unit, begin, end, free_call );
        if( lh ) {
            unit  = FIELD_ENTRY( lh, struct cnt_deque_unit, list_ );
            begin = unit->array_;
        } else {
            unit  = NULL;
        }
    }
}

void cnt_deque_free2( struct cnt_deque *cnd, cnt_deque_element_free free_call )
{
    if( cnd ) {
        cnt_deque_list_free( cnd, cnd->units_[DIRECT_TOP], free_call );
        free(cnd);
    }
}

void cnt_deque_free( struct cnt_deque *cnd )
{
    cnt_deque_free2( cnd, cnd->free_ );
}

