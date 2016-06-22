
#include <stdio.h>
#include <stdint.h>

#include "templ/arrays.h"
#include "templ/lists.h"
#include "lists/bilinked-list.h"
#include "inc/struct-fields.h"

array_define_type(int)
array_define_custom_type( const char *, string_array )

struct test {
    int  i_;
    char put_;
};

array_define_custom_type(struct test, test_array)

void show_array( const int_array_type *arr )
{
    size_t i;
    array_foreach( *arr, i ) {
        printf( "%d ", array_at( *arr, i ) );
    }
    printf( "\n" );
}

void show_array2( const string_array *arr )
{
    size_t i;
    array_foreach( *arr, i ) {
        printf( "%s ", array_at( *arr, i ) );
    }
    printf( "\n" );
}

typedef int deq_data;
#define MAX_DATA 64

typedef struct deq_node {
    struct bilinked_list_head head_;
    deq_data data_[MAX_DATA];
} deq_node;

typedef struct deque {
    deq_node *front_;
    deq_node *back_;
    deq_data *begin_;
    deq_data *end_;
    size_t    length_;
} deque;

void deq_init( deque *d )
{
    memset( d, 0, sizeof(*d) );
    d->front_ = d->back_ = malloc(sizeof(*d->back_));
    d->front_->head_.links_[0] = d->front_->head_.links_[1] = NULL;
    d->begin_ = d->end_ = &d->front_->data_[4];
    d->length_ = 0;
}

void deq_free( deque *d )
{
    while( d->front_ ) {
        deq_node *old = d->front_;
        d->front_ = FIELD_ENTRY( d->front_->head_.links_[1], deq_node, head_ );
        BILINKED_LIST_REMOVE( &old->head_ );
        free(old);
    }
    //free( d->front_ );
}

void deq_add_back( deque *d )
{
    deq_node *node =  calloc(sizeof(*d->back_), 1);
    BILINKED_LIST_INSERT( &d->back_->head_, &node->head_, 1 );
    d->end_ = &node->data_[0];
    d->back_ = node;
}

void deq_add_front( deque *d )
{
    deq_node *node = calloc(1, sizeof(*d->back_));
    BILINKED_LIST_INSERT( &d->front_->head_, &node->head_, 0 );
    d->begin_ = &node->data_[MAX_DATA];
    d->front_ = node;
}

void deq_push_front( deque *d, deq_data data )
{
    if( d->begin_ == &d->front_->data_[0] ) {
        deq_add_front( d );
    }
    *(--d->begin_) = data;
}

void deq_push_back( deque *d, deq_data data )
{
    *d->end_++ = data;
    if( d->end_ == &d->back_->data_[MAX_DATA] ) {
        deq_add_back( d );
    }
    ++d->length_;
}

void deq_pop_back( deque *d )
{
    if( d->end_ != d->begin_ ) {
        if( d->end_ == &d->back_->data_[0] ) {
            deq_node *old = d->back_;
            d->back_ = FIELD_ENTRY( old->head_.links_[0]->links_,
                                    deq_node, head_ );
            BILINKED_LIST_REMOVE( &old->head_ );
            free( old );
            d->end_ = &d->back_->data_[MAX_DATA-1];
        } else {
            --d->end_;
        }
        --d->length_;
    }
}

deq_data *deq_back( deque *d )
{
    if( d->end_ == &d->back_->data_[0] ) {
        deq_node *prev = FIELD_ENTRY( d->back_->head_.links_[0]->links_,
                                      deq_node, head_);
        return &prev->data_[MAX_DATA-1];
    } else {
        return (d->end_ - 1);
    }
}

deq_data *deq_front( deque *d )
{
    return (d->begin_);
}

static const size_t def_size = 10000;

int main( )
{

    deque d;
    deq_init( &d );

    for( int i = 0; i<10000; i++ ) {
        deq_push_back( &d, i );
    }

    for( int i = 0; i<500; i++ ) {
        deq_push_front( &d, i );
    }

    printf( "%d\n", *deq_back( &d ) );

    deq_free( &d );

    return 0;
    string_array   sa = array_init;

    test_array     ta = array_init;

    test_array_push_back( &ta, (struct test){ i_:1, put_: '1' } );
    test_array_push_back( &ta, (struct test){ i_:2, put_: '2' } );
    test_array_push_back( &ta, (struct test){ i_:3, put_: '3' } );
    test_array_push_back( &ta, (struct test){ i_:4, put_: '4' } );

    unsigned int j;
    array_foreach( ta, j ) {
        printf( "%d -> %c\n", array_at(ta, j).i_, array_at(ta, j).put_ );
    }

    string_array_push_back( &sa, "01" );
    string_array_push_back( &sa, "02" );
    string_array_push_back( &sa, "03" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "04" );
    string_array_push_back( &sa, "05" );
    string_array_push_back( &sa, "06" );

    array_push_back( sa, "01" );
    array_push_back( sa, "02" );
    array_push_back( sa, "03" );
    array_push_back( sa, "04" );
    array_push_back( sa, "04" );
    array_push_back( sa, "04" );
    array_push_back( sa, "04" );
    array_push_back( sa, "04" );
    array_push_back( sa, "05" );
    array_push_back( sa, "06" );

    int i;

    size_t position;

    array_bin_upper_bound_compare( sa, "04", strcmp, position );
    array_insert( sa, position, "!!", i );

    printf( "Index is %lu is end: %d\n",
            position, (position == array_lenght(sa)) );

    show_array2( &sa );

    array_free(sa);
    array_free(ta);

    return 0;
}
