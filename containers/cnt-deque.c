
#include <memory.h>

#include "cnt-deque.h"
#include "../memory/mm-array.h"
#include "../lists/bilinked-list.h"
#include "../inc/struct-fields.h"

struct cnt_deque_unit {
    struct bilinked_list_head list_;
    struct mm_array          *array_;
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

#define cnt_deque_is_on_top( cnd ) \
    ((cnd)->first_ == mm_array_begin( (cnd)->first_unit_->array_ ))

#define cnt_deque_is_on_bottom( cnd ) \
    ((cnd)->last_ == mm_array_end( (cnd)->last_unit_->array_ ))


#define cnt_deque_empty_local( cnd ) ((cnd)->first_ == (cnd)->last_)


struct cnt_deque_unit *cnt_deque_unit_create( struct cnt_deque* cnd,
                                              size_t elements )
{
    struct cnt_deque_unit *new_unit =
            (struct cnt_deque_unit *)calloc(1, sizeof(struct cnt_deque_unit) );
    if( new_unit ) {
        new_unit->array_ = mm_array_new2( elements, cnd->element_size_ );
        if( !new_unit->array_ ) {
            free( new_unit );
            new_unit = NULL;
        }
    }
    return new_unit;
}

void cnt_deque_unit_free_no_arr( struct cnt_deque_unit  *unit )
{
    mm_array_free( unit->array_ );
    free( unit );
}

void cnt_deque_unit_free( struct cnt_deque *cnd,
                          struct cnt_deque_unit  *unit,
                          void *begin, void *end,
                          cnt_deque_element_free free_call )
{
    void *arr_end = mm_array_end( unit->array_ );

    while ( begin != end && begin != arr_end ) {
        free_call( begin );
        begin = cnt_deque_element_next( cnd, begin );
    }
    cnt_deque_unit_free_no_arr( unit );
}


struct cnt_deque* cnt_deque_new2( size_t element_size,
                                  cnt_deque_element_free free_call )
{
    struct cnt_deque *new_deq =
            (struct cnt_deque *)malloc(sizeof(struct cnt_deque));
    if( new_deq ) {
        memset( new_deq, 0, sizeof(struct cnt_deque) );
        new_deq->element_size_ = element_size;
        new_deq->free_         = free_call;
        struct cnt_deque_unit *unit = cnt_deque_unit_create( new_deq, 8 );
        if( unit ) {
            new_deq->first_unit_ = new_deq->last_unit_ = unit;
            new_deq->first_ = new_deq->last_ = mm_array_at( unit->array_, 4 );
        } else {
            free(new_deq);
            new_deq = NULL;
        }
    }

    return new_deq;
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

int cnt_deque_push_front2(struct cnt_deque *cnd, void *element,
                          cnt_deque_element_copy copy_call)
{
    int res = 0;
    if( cnt_deque_is_on_top( cnd ) ) {
        struct cnt_deque_unit *new_top = cnt_deque_unit_create( cnd, 16 );
        if( new_top ) {
            bilinked_list_insert( &cnd->first_unit_->list_,
                                  &new_top->list_, BILINK_DIRECT_BACKWARD );
            cnd->first_unit_ = new_top;
            cnd->first_ = mm_array_end( new_top->array_ );
            res = 1;
        }
    } else {
        res = 1;
    }
    if( res ) {
        cnd->first_ = cnt_deque_element_prev( cnd, cnd->first_ );
        if( copy_call )
            copy_call( cnd->first_, element, cnd->element_size_ );
        ++cnd->count_;
    }
    return res;
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
        struct mm_array *first_arr = cnd->first_unit_->array_;
        if( mm_array_is_end(first_arr, new_top) && new_top!=cnd->last_) {
            struct cnt_deque_unit  *next_unit =
                    bilinked_list_next(&cnd->first_unit_->list_,
                                       BILINK_DIRECT_FORWARD);
            bilinked_list_remove( &cnd->first_unit_->list_ );
            cnt_deque_unit_free_no_arr( cnd->first_unit_ );
            cnd->first_unit_ = next_unit;
            new_top = mm_array_begin( next_unit->array_ );
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
        struct mm_array *last_arr = cnd->last_unit_->array_;
        if( mm_array_is_begin(last_arr, new_last) && new_last!=cnd->first_) {
            struct cnt_deque_unit  *next_unit =
                    bilinked_list_next(&cnd->last_unit_->list_,
                                       BILINK_DIRECT_BACKWARD);
            bilinked_list_remove( &cnd->last_unit_->list_ );
            cnt_deque_unit_free_no_arr( cnd->last_unit_ );
            cnd->last_unit_ = next_unit;
            new_last = mm_array_end( next_unit->array_ );
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

int cnt_deque_push_back2( struct cnt_deque *cnd, void *element,
                          cnt_deque_element_copy copy_call )
{
    int res = 0;
    if( cnt_deque_is_on_bottom( cnd ) ) {
        struct cnt_deque_unit *new_bottom = cnt_deque_unit_create( cnd, 16 );
        if( new_bottom ) {
            bilinked_list_insert( &cnd->last_unit_->list_,
                                  &new_bottom->list_, BILINK_DIRECT_FORWARD );
            cnd->last_unit_ = new_bottom;
            cnd->last_ = mm_array_begin( new_bottom->array_ );
            res = 1;
        }
    } else {
        res = 1;
    }
    if( res ) {
        if( copy_call )
            copy_call( cnd->last_, element, cnd->element_size_ );
        cnd->last_ = cnt_deque_element_next( cnd, cnd->last_ );
        ++cnd->count_;
    }
    return res;

}

int cnt_deque_push_back ( struct cnt_deque *cnd, void *element )
{
    return cnt_deque_push_back2( cnd, element, NULL );
}


struct cnt_deque* cnt_deque_new( size_t element_size )
{
    return cnt_deque_new2( element_size, NULL );
}

void  cnt_deque_empty( struct cnt_deque *cnd )
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
            begin = mm_array_begin( unit->array_ );
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

