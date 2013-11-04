
#include <memory.h>

#include "cnt-deque.h"
#include "../lists/bilinked-list.h"
#include "../inc/struct-fields.h"

struct cnt_deque_unit {
    struct bilinked_list_head list_;
    char                     *array_;
    size_t                    length_;
};

struct cnt_deque {
    struct cnt_deque_unit  *first_unit_;
    struct cnt_deque_unit  *last_unit_;
    void                   *first_;
    void                   *last_;
    size_t                  element_size_;
    size_t                  count_;
    cnt_deque_element_free  free_;
};

#define cnt_deque_element_next( cnd, ptr ) \
            (((char *)ptr) + ((cnd)->element_size_))

#define cnt_deque_element_prev( cnd, ptr ) \
            (((char *)ptr) - ((cnd)->element_size_))

#define cnt_deque_block_begin( unit ) ((unit)->array_)
#define cnt_deque_block_at( unit, element_size, count ) \
    (((unit)->array_ + ((element_size) * count)))

#define cnt_deque_block_end( unit ) ((unit)->array_ + (unit)->length_)

#define cnt_deque_block_is_end( unit, ptr ) \
    (cnt_deque_block_end( unit ) == (ptr))

#define cnt_deque_is_on_top( cnd ) \
    ((cnd)->first_ == (cnd)->first_unit_->array_ )

#define cnt_deque_is_on_bottom( cnd ) \
    ((cnd)->last_ == ( (cnd)->last_unit_->array_ + (cnd)->last_unit_->length_))

#define cnt_deque_empty_local( cnd ) ((cnd)->first_ == (cnd)->last_)

#define cnt_deque_def_inc(size) (size + (size >> 1))

size_t cnt_deque_calc_prefer_size( size_t old_size, size_t element_size )
{
    size_t new_count = cnt_deque_def_inc(old_size / element_size);
    return new_count;
}

#define cnt_deque_pref_first_size( cnd ) \
    cnt_deque_calc_prefer_size((cnd)->first_unit_->length_,(cnd)->element_size_)

#define cnt_deque_pref_last_size( cnd ) \
    cnt_deque_calc_prefer_size((cnd)->last_unit_->length_,(cnd)->element_size_)

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
    void *arr_end = cnt_deque_block_end( unit );

    while ( begin != end && begin != arr_end ) {
        free_call( begin );
        begin = cnt_deque_element_next( cnd, begin );
    }
    cnt_deque_unit_free_no_arr( unit );
}

void cnt_deque_init_unit_position( struct cnt_deque *cnd, size_t reserve,
                                   enum cnt_deque_start_point position)
{
    switch( position ) {
    case DEQUE_START_TOP:
        cnd->first_ = cnd->last_ =
                cnt_deque_block_begin( cnd->first_unit_ );
        break;
    case DEQUE_START_MIDDLE:
        cnd->first_ = cnd->last_ =
                cnt_deque_block_at( cnd->first_unit_, cnd->element_size_,
                                    reserve >> 1);
        break;
    case DEQUE_START_BOTTOM:
        cnd->first_ = cnd->last_ =
                cnt_deque_block_end( cnd->first_unit_ );
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
            new_deq->first_unit_ = new_deq->last_unit_ = unit;
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
    return cnt_deque_empty_local(cnd) ? NULL : cnd->first_;
}

void *cnt_deque_back( struct cnt_deque *cnd )
{
    void *ptr = NULL;
    if( !cnt_deque_empty_local(cnd) ) {
        ptr = cnt_deque_element_prev( cnd, cnd->last_ );
    }
    return ptr;
}

void *cnt_deque_create_front( struct cnt_deque *cnd )
{
    void *new_front_ptr = NULL;
    int res = 0;
    if( cnt_deque_is_on_top( cnd ) ) {
        struct cnt_deque_unit *new_top =
                cnt_deque_unit_create( cnd, cnt_deque_pref_first_size(cnd) );

        if( new_top ) {
            bilinked_list_insert( &cnd->first_unit_->list_,
                                  &new_top->list_, BILINK_DIRECT_BACKWARD );
            cnd->first_unit_ = new_top;
            cnd->first_ = cnt_deque_block_end( new_top );
            res = 1;
        }
    } else {
        res  = 1;
    }
    if( res ) {
        cnd->first_ = cnt_deque_element_prev( cnd, cnd->first_ );
        new_front_ptr = cnd->first_;
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

int cnt_deque_push_front( struct cnt_deque *cnd, void *element )
{
    return cnt_deque_push_front2( cnd, element, memcpy );
}

int cnt_deque_pop_front2( struct cnt_deque *cnd,
                          cnt_deque_element_free free_call)
{
    int res = 0;
    if( !cnt_deque_empty_local(cnd) ) {
        res = 1;
        void *old_top = cnd->first_;
        void *new_top = cnt_deque_element_next( cnd, old_top );
        if( free_call ) {
            free_call( old_top );
        }
        char *first_arr = cnd->first_unit_->array_;
        if( cnt_deque_block_is_end(cnd->first_unit_, new_top)
                                && new_top!=cnd->last_)
        {
            struct cnt_deque_unit  *next_unit =
                    bilinked_list_next(&cnd->first_unit_->list_,
                                       BILINK_DIRECT_FORWARD);
            bilinked_list_remove( &cnd->first_unit_->list_ );
            cnt_deque_unit_free_no_arr( cnd->first_unit_ );
            cnd->first_unit_ = next_unit;
            new_top = next_unit->array_;
        }
        cnd->first_ = new_top;
        --cnd->count_;
    }
    return res;
}

int cnt_deque_pop_front ( struct cnt_deque *cnd )
{
    cnt_deque_pop_front2( cnd, cnd->free_ );
}

int cnt_deque_pop_back2 ( struct cnt_deque *cnd,
                          cnt_deque_element_free free_call )
{
    int res = 0;
    if( !cnt_deque_empty_local(cnd) ) {
        res = 1;
        void *new_last = cnt_deque_element_prev( cnd, cnd->last_ );
        if( free_call ) {
            free_call( new_last );
        }
        char *last_arr = cnd->last_unit_->array_;
        if( (last_arr == new_last) && new_last!=cnd->first_) {
            struct cnt_deque_unit  *next_unit =
                    bilinked_list_next(&cnd->last_unit_->list_,
                                       BILINK_DIRECT_BACKWARD);
            bilinked_list_remove( &cnd->last_unit_->list_ );
            cnt_deque_unit_free_no_arr( cnd->last_unit_ );
            cnd->last_unit_ = next_unit;
            new_last = cnt_deque_block_end( next_unit );
        }
        cnd->last_ = new_last;
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
    if( cnt_deque_is_on_bottom( cnd ) ) {
        struct cnt_deque_unit *new_bottom =
                cnt_deque_unit_create( cnd, cnt_deque_pref_last_size(cnd) );

        if( new_bottom ) {
            bilinked_list_insert( &cnd->last_unit_->list_,
                                  &new_bottom->list_, BILINK_DIRECT_FORWARD );
            cnd->last_unit_ = new_bottom;
            cnd->last_ = new_bottom->array_;
            res = 1;
        }
    } else {
        res = 1;
    }
    if( res ) {
        new_back = cnd->last_;
        cnd->last_ = cnt_deque_element_next( cnd, cnd->last_ );
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
    return cnt_deque_empty_local(cnd);
}

size_t cnt_deque_size ( struct cnt_deque *cnd )
{
    return cnd->count_;
}

void cnt_deque_list_free( struct cnt_deque *cnd,
                          struct cnt_deque_unit  *unit,
                          cnt_deque_element_free free_call )
{
    void *begin = cnd->first_;
    void *end   = cnd->last_;
    while( unit ) {
        struct bilinked_list_head *lh =
                bilinked_list_next( &unit->list_, BILINK_DIRECT_FORWARD );
        cnt_deque_unit_free( cnd, unit, begin, end, free_call );
        if( lh ) {
            unit  = field_entry( lh, struct cnt_deque_unit, list_ );
            begin = unit->array_;
        } else {
            unit  = NULL;
        }
    }
}

void cnt_deque_free2( struct cnt_deque *cnd, cnt_deque_element_free free_call )
{
    if( cnd ) {
        cnt_deque_list_free( cnd, cnd->first_unit_, free_call );
        free(cnd);
    }
}

void cnt_deque_free( struct cnt_deque *cnd )
{
    cnt_deque_free2( cnd, cnd->free_ );
}

