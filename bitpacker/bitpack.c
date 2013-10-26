#include <limits.h>
#include <memory.h>
#include <stdlib.h>

#include <stdio.h>

#include "bitpack.h"
#include "mem-block.h"

typedef size_t          value_type;
typedef unsigned char   container_type;

unsigned char make_mask( unsigned count )
{
    return (unsigned char)(((1 << count) - 1) & 0xFF);
}

char * byte_to_bits( container_type b, char *storage )
{
    int i;
    for( i=0; i<8; ++i ) {
        storage[i] = (b & (1 << i)) ? '1' : '0';
    }
    storage[8] = '\0';
    return storage;
}

unsigned unpack_bits( value_type *val, unsigned count,
                      unsigned char dst, unsigned *size )
{
    unsigned tail  = 0;
    unsigned shift = 0;

    value_type val_ = *val;
    unsigned size_  = CHAR_BIT - *size;

    if( size_ == 0 ) return count;

    if( size_ < count ) {
        tail = count - size_;
        size_ = 0;
        count -= tail;
    } else {
        shift = (size_ -= count);
    }

    val_ = (val_ << count) | (( dst >> shift ) & make_mask( count ));

    *val  = val_;
    *size = CHAR_BIT - size_;

    return tail;
}

unsigned pack_bits( value_type val, unsigned count,
                    unsigned char *dst, unsigned *size)
{
    static const unsigned char_bits = CHAR_BIT;

    unsigned size_      = *size;
    unsigned char dst_  = *dst;

    unsigned new_fill = size_ + count;
    unsigned tail = 0;
    unsigned shift = 0;

    if( new_fill >= char_bits ) {
        size_ = char_bits;
        tail  = new_fill - char_bits;
        val >>= tail;
        count -= tail;
    } else {
        shift = char_bits - new_fill;
        size_ = new_fill;
    }

    dst_ |= ((val & make_mask( count )) << shift);

    *dst  = dst_;
    *size = size_;

    return tail;
}

struct tail_info {
    container_type current_;
    unsigned       filling_;
};

struct bit_pack_data {
    container_type   *data_;
    value_type        current_len_;
    value_type        current_pos_;
    struct tail_info  ti_;
};

typedef struct bit_pack_data  bit_pack_data_type;

void bp_dump( struct bit_pack_data *bpd )
{
    printf( "Current size: %d\n", bpd->current_len_ );
    printf( "Current pos: %d\n", bpd->current_pos_ );
    printf( "Current fil: %d\n", bpd->ti_.filling_ );
    printf( "Current dump: \n");
    char b2b[9] = {0};
    do {
        value_type i;
        for( i=0; i <= bpd->current_pos_;  ++i ) {
            printf( " %s", byte_to_bits(bpd->data_[i], b2b));
        }
    } while(0);
    printf( "\n");
}


int bp_realloc_data( bit_pack_data_type *bpd )
{
    value_type new_len = bpd->current_len_ * 1.5;
    container_type *new_data = (container_type *)
            malloc( new_len * sizeof( container_type ) );

    if( NULL == new_data ) return 0;

    memcpy( new_data, bpd->data_, bpd->current_len_ );
    free( bpd->data_ );
    bpd->data_ = new_data;
    bpd->current_len_ = new_len;
    return 1;
}

bit_pack_data_type *bp_new_bitpack_data( )
{
    bit_pack_data_type *new_data = (struct bit_pack_data *)
                                    malloc(sizeof(bit_pack_data_type));

    if( NULL == new_data ) return NULL;

    new_data->current_len_ = 8;
    new_data->current_pos_ = 0;

    new_data->data_ = (container_type *)
            malloc(new_data->current_len_ * sizeof( container_type ));

    if( NULL == new_data->data_ ) {
        free( new_data );
        return NULL;
    }

    new_data->ti_.current_ = 0;
    new_data->ti_.filling_ = 0;

    return new_data;
}

void bp_delete_bitpack_data( bit_pack_data_type *bpd )
{
    if( NULL == bpd ) return;
    free( bpd->data_ );
    free( bpd );
}

int bp_is_space_enough( struct bit_pack_data *bpd, unsigned bit_count )
{
    return (bpd->current_pos_ + (bit_count / CHAR_BIT) + 1) <= bpd->current_len_;
}

int bp_push( struct bit_pack_data *bpd )
{
	if( bpd->current_pos_ + 1 >= bpd->current_len_ ) {
		if( !bp_realloc_data( bpd ) ) return 0;
	}
	bpd->data_[bpd->current_pos_++] = bpd->ti_.current_;
	bpd->ti_.current_ = 0;
	bpd->ti_.filling_ = 0;
	return 1;
}

int bp_add_bits(struct bit_pack_data *bpd, unsigned value, unsigned bit_count)
{
    unsigned tail = bit_count;
    do {
        tail = pack_bits( value, tail,
                          &bpd->ti_.current_, &bpd->ti_.filling_ );
        if( tail != 0 ) {
        	if(!bp_push( bpd )) return 0;
        }
    } while ( tail );

    if( bpd->ti_.filling_ == CHAR_BIT )
        return bp_push( bpd );

    return 1;
}

unsigned bp_get_padd( struct bit_pack_data *bpd )
{
    return bpd->ti_.filling_ ? (CHAR_BIT - bpd->ti_.filling_) : 0;
}

size_t bp_get_size( struct bit_pack_data *bpd )
{
    return bpd->current_pos_ + ((bpd->ti_.filling_ != 0) ? 1 : 0);
}

