
#include <memory.h>

#include "cnt-deque.h"
#include "../lists/bilinked-list.h"
#include "../inc/struct-fields.h"

enum cnt_deque_direction {
     SIDE_FRONT = 0
    ,SIDE_BACK  = 1
};

struct cnt_deque_unit {
    struct bilinked_list_head list_;
    void                     *border_[2];
};

struct cnt_deque_side {
    struct cnt_deque_unit  *unit_;
    void                   *ptr_;
};

struct cnt_deque {
    struct cnt_deque_side   sides_[2];
    size_t                  element_size_;
    size_t                  count_;
    cnt_deque_element_free  free_;
};

struct cnt_deque_iterator
{
    const struct cnt_deque *parent_;
    struct cnt_deque_unit  *unit_;
    void                   *ptr_;
    short  next_;
    short  side_;
};


#define CNT_DEQUE_ELEMENT_NEXT( ptr, element_size ) \
            (((char *)ptr) + (element_size))

#define CNT_DEQUE_ELEMENT_PREV( ptr, element_size ) \
            (((char *)ptr) - (element_size))

#define CNT_DEQUE_BLOCK_BEGIN( unit ) ((unit)->border_[0])

#define CNT_DEQUE_BLOCK_AT( unit, element_size, count ) \
    ((char *)(unit)->border_[0] + ((element_size) * (count)))

#define CNT_DEQUE_BLOCK_END( unit ) ((unit)->border_[1])

#define CNT_DEQUE_BLOCK_SIDE( unit, side ) ((unit)->border_[!(side)])

#define CNT_DEQUE_BLOCK_IS_SIDE( unit, ptr, side )       \
        CNT_DEQUE_BLOCK_SIDE(unit, side) == (ptr)

#define CNT_DEQUE_IS_BORDER( cnd, side )            \
        ((cnd)->sides_[side].ptr_ ==                \
         (cnd)->sides_[side].unit_->border_[side] )

//#define CNT_DEQUE_DEF_INC(size) ((size))
#define CNT_DEQUE_DEF_INC(size) ((size) + ((size) >> 1))


static void *cnt_deque_malloc(size_t size)
{
    void *ptr = malloc( size );
//    printf( "alloc %p\n", ptr );
    return ptr;
}

static void cnt_deque_freemem(void *ptr)
{
    free( ptr );
//    printf( "free %p\n", ptr );
}

static void *cnt_deque_memcpy(void *dest, const void *src, size_t n)
{
    return memcpy(dest, src, n);
}

struct cnt_deque_unit *cnt_deque_unit_create( struct cnt_deque* cnd,
                                              size_t elements )
{
    struct cnt_deque_unit *new_unit =
      (struct cnt_deque_unit *)cnt_deque_malloc(sizeof(struct cnt_deque_unit));
    if( new_unit ) {
        size_t length = elements * cnd->element_size_;
        void *new_border = cnt_deque_malloc( length );
        new_unit->border_[0] = new_border;
        new_unit->border_[1] = ((char *)new_border) + length;
        new_unit->list_.links_[0] = new_unit->list_.links_[1] = NULL;
        if( !new_border ) {
            cnt_deque_freemem( new_unit );
            new_unit = NULL;
        }
    }
    return new_unit;
}

void cnt_deque_init_unit_position( struct cnt_deque *cnd, size_t reserve,
                                   enum cnt_deque_start_point position)
{
    void *ptr_new = NULL;
    size_t size = cnd->element_size_;
    switch( position ) {
    case DEQUE_START_TOP:
        ptr_new =
        CNT_DEQUE_ELEMENT_NEXT(cnd->sides_[SIDE_FRONT].unit_->border_[0], size);

        break;
    case DEQUE_START_BOTTOM:
        ptr_new =
        CNT_DEQUE_ELEMENT_PREV(cnd->sides_[SIDE_FRONT].unit_->border_[1], size);
        break;

    default:
        ptr_new = CNT_DEQUE_BLOCK_AT(cnd->sides_[SIDE_FRONT].unit_,
                                     cnd->element_size_, reserve >> 1);
        break;
    }
    cnd->sides_[SIDE_FRONT].ptr_ = cnd->sides_[SIDE_BACK].ptr_ =  ptr_new;
}

struct cnt_deque *cnt_deque_new_all( size_t element_size,
                                     size_t init_reserve,
                                     cnt_deque_element_free free_call,
                                     enum cnt_deque_start_point position)
{
    struct cnt_deque *new_deq =
            (struct cnt_deque *)cnt_deque_malloc(sizeof(struct cnt_deque));
    if( new_deq ) {
        memset( new_deq, 0, sizeof(struct cnt_deque) );
        new_deq->element_size_ = element_size;
        new_deq->free_         = free_call;
        struct cnt_deque_unit *unit =
                cnt_deque_unit_create( new_deq,
                                       init_reserve ? init_reserve + 1: 8);
        if( unit ) {
            new_deq->sides_[SIDE_FRONT].unit_ =
                new_deq->sides_[SIDE_BACK].unit_ = unit;
            cnt_deque_init_unit_position( new_deq, init_reserve + 1, position );
        } else {
            cnt_deque_freemem(new_deq);
            new_deq = NULL;
        }
    }
    return new_deq;
}

struct cnt_deque *cnt_deque_new_reserved2( size_t element_size,
                                           size_t init_reserve,
                                           cnt_deque_element_free free_call)
{
    return cnt_deque_new_all( element_size, init_reserve,
                              free_call, DEQUE_START_MIDDLE );
}

struct cnt_deque *cnt_deque_new_reserved( size_t element_size,
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

struct cnt_deque *cnt_deque_new2( size_t element_size,
                                  cnt_deque_element_free free_call )
{
    return cnt_deque_new_all( element_size, 8, free_call, DEQUE_START_MIDDLE );
}

struct cnt_deque *cnt_deque_new( size_t element_size )
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
    return cnd->sides_[SIDE_FRONT].ptr_;
}

void *cnt_deque_back( struct cnt_deque *cnd )
{
    void *ptr = NULL;
    ptr = CNT_DEQUE_ELEMENT_PREV( cnd->sides_[SIDE_BACK].ptr_,
                                  cnd->element_size_ );
    return ptr;
}

static int cnt_deque_extend_side( struct cnt_deque *cnd, int dir )
{
    struct cnt_deque_side *side       = &cnd->sides_[dir];
    struct cnt_deque_unit *new_unit   =
            FIELD_ENTRY( BILINKED_LIST_STEP( &side->unit_->list_, dir ),
                         struct cnt_deque_unit, list_);

    if( !new_unit ) {
        struct bilinked_list_head *other_list =
                BILINKED_LIST_STEP( &cnd->sides_[!dir].unit_->list_, !dir );
        if( other_list ) {
            new_unit = FIELD_ENTRY( other_list, struct cnt_deque_unit, list_ );
            BILINKED_LIST_REMOVE( other_list );
            other_list->links_[0] = other_list->links_[1] = NULL;
        } else {
            const size_t old_size = (side->unit_->border_[1]-
                                     side->unit_->border_[0]) /
                                                cnd->element_size_;
            const size_t new_size = CNT_DEQUE_DEF_INC(old_size);
            new_unit = cnt_deque_unit_create( cnd, new_size );
        }
        BILINKED_LIST_INSERT( &side->unit_->list_, &new_unit->list_, dir );
    }

    if( new_unit ) {
        side->unit_ = new_unit;
        side->ptr_  = CNT_DEQUE_BLOCK_SIDE( new_unit, dir );
    }

    return new_unit != NULL;
}

static void *cnt_dec_ptr_call( void **ptr, size_t size )
{
    char *tmp = (char *)*ptr;
    tmp  -= size;
    *ptr  = tmp;
    return tmp;
}

static void *cnt_inc_ptr_call( void **ptr, size_t size )
{
    char *tmp = (char *)*ptr;
    *ptr = tmp + size;
    return tmp;
}

static void *cnt_deque_change_side( struct cnt_deque *cnd, int dir )
{
    typedef void *(* fix_ptr_call)( void **, size_t );
    static const fix_ptr_call ptr_calls[2] = {
        cnt_dec_ptr_call, cnt_inc_ptr_call
    };

    void *new_ptr = NULL;
    int res = 1;
    if( CNT_DEQUE_IS_BORDER( cnd, dir ) ) {
        res = cnt_deque_extend_side( cnd, dir );
    }
    if( res ) {
        struct cnt_deque_side *side = &cnd->sides_[dir];
        new_ptr = ptr_calls[dir]( &side->ptr_, cnd->element_size_ );
        ++cnd->count_;
    }
    return new_ptr;
}

void *cnt_deque_create_front( struct cnt_deque *cnd )
{
    return cnt_deque_change_side( cnd, SIDE_FRONT );
}

int cnt_deque_push_front2(struct cnt_deque *cnd, const void *element,
                          cnt_deque_element_copy copy_call)
{
    void *new_front = cnt_deque_create_front( cnd );
    if( new_front && copy_call) {
        copy_call( new_front, element, cnd->element_size_ );
    }
    return new_front != NULL;
}

int cnt_deque_push_front( struct cnt_deque *cnd, const void *element )
{
    return cnt_deque_push_front2( cnd, element, cnt_deque_memcpy );
}

static void cnt_deque_pop_side( struct cnt_deque *cnd, int dir,
                         cnt_deque_element_free free_call )

{
    struct cnt_deque_side *side = &cnd->sides_[dir];

    void *old_and_new_side[2];
    void *new_side;
    size_t elem_size = cnd->element_size_;

    old_and_new_side[0] = side->ptr_;
    old_and_new_side[1] = new_side =
            dir ? CNT_DEQUE_ELEMENT_PREV( side->ptr_, elem_size )
                : CNT_DEQUE_ELEMENT_NEXT( side->ptr_, elem_size );

    if( free_call ) {
        free_call( old_and_new_side[dir] );
    }

    if( CNT_DEQUE_BLOCK_IS_SIDE(side->unit_, new_side, dir)
            && new_side != cnd->sides_[!dir].ptr_ )
    {
        struct bilinked_list_head  *old_list = &side->unit_->list_;
        struct bilinked_list_head  *next_unit =
                BILINKED_LIST_STEP(&side->unit_->list_, !dir);
        side->unit_ = FIELD_ENTRY(next_unit, struct cnt_deque_unit, list_);
        new_side    = CNT_DEQUE_BLOCK_SIDE(side->unit_, !dir);

        if( NULL != (old_list = old_list->links_[dir]) ) {
            struct cnt_deque_unit *old_unit =
                    FIELD_ENTRY(old_list, struct cnt_deque_unit, list_);
            BILINKED_LIST_REMOVE( old_list );
            cnt_deque_freemem( old_unit->border_[0] );
            cnt_deque_freemem( old_unit );
        }

    }

    side->ptr_ = new_side;
    --cnd->count_;
}

void cnt_deque_pop_front2( struct cnt_deque *cnd,
                           cnt_deque_element_free free_call)
{
    cnt_deque_pop_side( cnd, SIDE_FRONT, free_call );
}

void cnt_deque_pop_front ( struct cnt_deque *cnd )
{
    cnt_deque_pop_front2( cnd, cnd->free_ );
}

void cnt_deque_pop_back2 ( struct cnt_deque *cnd,
                          cnt_deque_element_free free_call )
{
    cnt_deque_pop_side( cnd, SIDE_BACK, free_call );
}

void cnt_deque_pop_back ( struct cnt_deque *cnd )
{
    return cnt_deque_pop_back2( cnd, cnd->free_ );
}

void *cnt_deque_create_back( struct cnt_deque *cnd)
{
    return cnt_deque_change_side( cnd, SIDE_BACK );
}

int cnt_deque_push_back2( struct cnt_deque *cnd, const void *element,
                          cnt_deque_element_copy copy_call )
{
    void *new_back = cnt_deque_create_back( cnd );
    if( new_back && copy_call )
        copy_call( new_back, element, cnd->element_size_ );
    return new_back != NULL;
}

int cnt_deque_push_back ( struct cnt_deque *cnd, const void *element )
{
    return cnt_deque_push_back2( cnd, element, cnt_deque_memcpy );
}

int cnt_deque_empty( struct cnt_deque *cnd )
{
    return cnd->sides_[SIDE_FRONT].ptr_ == cnd->sides_[SIDE_BACK].ptr_;
}

size_t cnt_deque_size ( struct cnt_deque *cnd )
{
    return cnd->count_;
}

static int cnt_deque_unit_free( struct cnt_deque *cnd,
                         struct cnt_deque_unit  *unit,
                         void *begin, const void *end,
                         cnt_deque_element_free free_call )
{
    const void *block_end = CNT_DEQUE_BLOCK_END( unit );
    size_t element_size = cnd->element_size_;
    while( begin != end && begin != block_end ) {
        free_call( begin );
        begin = ((char *)begin) + element_size;
    }
    return (begin != end);
}

static void cnt_deque_list_free( struct cnt_deque *cnd,
                                 cnt_deque_element_free free_call )
{
    // free elements
    struct cnt_deque_unit     *unit       = NULL;
    struct cnt_deque_side     *front_side = &cnd->sides_[SIDE_FRONT];
    struct bilinked_list_head *head       = &front_side->unit_->list_;

    if( free_call ) {
        void *begin     = NULL;
        const void *end = cnd->sides_[SIDE_BACK].ptr_;
        while( head ) {
            unit = FIELD_ENTRY( head, struct cnt_deque_unit, list_ );
            begin = begin ? unit->border_[0]
                          : front_side->ptr_;
            head = BILINKED_LIST_NEXT( head );
            if(!cnt_deque_unit_free( cnd, unit, begin, end, free_call )) {
                head = NULL;
            }
        }
    }
    // free units
    head = &front_side->unit_->list_;
    while( head->links_[0] ) head = head->links_[0];
    while( head ) {
        unit = FIELD_ENTRY( head, struct cnt_deque_unit, list_ );
        head = BILINKED_LIST_NEXT( head );
        cnt_deque_freemem( unit->border_[0] );
        cnt_deque_freemem( unit );
    }
}

void cnt_deque_free2( struct cnt_deque *cnd, cnt_deque_element_free free_call )
{
    if( cnd ) {
        cnt_deque_list_free( cnd, free_call );
        cnt_deque_freemem(cnd);
    }
}

void cnt_deque_free( struct cnt_deque *cnd )
{
    if( cnd ) cnt_deque_free2( cnd, cnd->free_ );
}

static struct cnt_deque_iterator
    *cnt_deque_iterator_both(const struct cnt_deque *cnd, short direction)
{
    struct cnt_deque_iterator *iter = (struct cnt_deque_iterator *)
            cnt_deque_malloc(sizeof(struct cnt_deque_iterator));
    if( iter ) {
        iter->parent_ = cnd;
        iter->side_   = !direction;
        iter->next_   = direction;
        iter->unit_   = cnd->sides_[!direction].unit_;
        iter->ptr_    = cnd->sides_[!direction].ptr_;
    }
    return iter;
}

struct cnt_deque_iterator
        *cnt_deque_reverse_iterator_new(const  struct cnt_deque *cd)
{
    return cnt_deque_iterator_both( cd, 0 );
}

struct cnt_deque_iterator
        *cnt_deque_iterator_new( const struct cnt_deque *cnd )
{
    return cnt_deque_iterator_both( cnd, 1 );
}

struct cnt_deque_iterator
        *cnt_deque_iterator_clone(const struct cnt_deque_iterator *src)
{
    struct cnt_deque_iterator *iter = (struct cnt_deque_iterator *)
            cnt_deque_malloc(sizeof(struct cnt_deque_iterator));
    if( iter ) {
       cnt_deque_memcpy( iter, src, sizeof(struct cnt_deque_iterator) );
    }
    return iter;
}

int cnt_deque_iterator_next( struct cnt_deque_iterator *iter )
{
    size_t elem_size = iter->parent_->element_size_;
    void *ptr = iter->next_ ? CNT_DEQUE_ELEMENT_NEXT(iter->ptr_, elem_size)
                            : CNT_DEQUE_ELEMENT_PREV(iter->ptr_, elem_size);
    if( CNT_DEQUE_BLOCK_IS_SIDE( iter->unit_, ptr, iter->side_ ) ) {
        struct bilinked_list_head *lh =
                BILINKED_LIST_STEP( &iter->unit_->list_, iter->next_ );
        if( lh ) {
            iter->unit_ = FIELD_ENTRY( lh, struct cnt_deque_unit, list_ );
            ptr = CNT_DEQUE_BLOCK_SIDE( iter->unit_, iter->next_ );
        }
    }
    iter->ptr_ = ptr;
    return 1;
}

int cnt_deque_iterator_end( struct cnt_deque_iterator *iter )
{
    return iter->ptr_ == iter->parent_->sides_[iter->next_].ptr_;
}

void *cnt_deque_iterator_get( struct cnt_deque_iterator *iter )
{
    return iter->next_ ? iter->ptr_
                       : CNT_DEQUE_ELEMENT_PREV(iter->ptr_,
                                                iter->parent_->element_size_);
}

void cnt_deque_iterator_free( struct cnt_deque_iterator *iter )
{
    if( iter ) {
        cnt_deque_freemem( iter );
    }
}
