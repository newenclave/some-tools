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
    for( i=8; i>0; --i ) {
        storage[8-i] = (b & (1 << (i-1))) ? '1' : '0';
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
    struct mem_block_data  *data_;
    struct tail_info        ti_;
};

typedef struct bit_pack_data  bit_pack_data_type;

void bp_dump( struct bit_pack_data *bpd )
{
    printf( "Current size: %d\n", mem_block_capacity(bpd->data_) );
    printf( "Current pos: %d\n",  mem_block_size(bpd->data_) );
    printf( "Current fil: %d\n",  bpd->ti_.filling_ );
    printf( "Current dump: \n");
    char b2b[9] = {0};
    do {
        value_type i;
        const char *data = (const char *)mem_block_data(bpd->data_);
        for( i=0; i < mem_block_size(bpd->data_);  ++i ) {
            printf( " S%s", byte_to_bits(data[i], b2b));
        }
        printf( " T%s", byte_to_bits(bpd->ti_.current_, b2b));
    } while(0);
    printf( "\n");
}


bit_pack_data_type *bp_new_bitpack_data( )
{
    bit_pack_data_type *new_data = (struct bit_pack_data *)
                                    malloc(sizeof(bit_pack_data_type));

    if( NULL == new_data ) return NULL;

    new_data->data_ = mem_block_new(0);

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
    mem_block_free(bpd->data_);
    free( bpd );
}

int bp_is_space_enough( struct bit_pack_data *bpd, unsigned bit_count )
{
    return 1;
}

int bp_add_bits(struct bit_pack_data *bpd, unsigned value, unsigned bit_count)
{
    struct tail_info tmp_ti;
    struct mem_block_data *tmp_data = bpd->data_;
    unsigned tail = bit_count;

    tmp_ti.current_ = bpd->ti_.current_;
    tmp_ti.filling_ = bpd->ti_.filling_;

    if( mem_block_available( bpd->data_ ) <= (bit_count / CHAR_BIT)) {
        tmp_data = mem_block_new(0);
        if( NULL == tmp_data ) {
            return 0;
        }
        mem_block_reserve( tmp_data, 8 );
    }

    do {
        tail = pack_bits( value, tail,
                         &tmp_ti.current_, &tmp_ti.filling_ );

        if( tmp_ti.filling_ == CHAR_BIT ) {
            if( 0 == mem_block_push_back(tmp_data, tmp_ti.current_)) {
                mem_block_free( tmp_data );
                return 0;
            }
            tmp_ti.current_ = 0;
            tmp_ti.filling_ = 0;
        }
    } while ( tail != 0 );

    if( bpd->data_ != tmp_data ) { // new block
        int result = 0;
        if( 0 != mem_block_concat2( bpd->data_, tmp_data )) {
            bpd->ti_.current_ = tmp_ti.current_;
            bpd->ti_.filling_ = tmp_ti.filling_;
            result = 1;
        }
        mem_block_free( tmp_data );
        return result;
    } else {
        bpd->ti_.current_ = tmp_ti.current_;
        bpd->ti_.filling_ = tmp_ti.filling_;
    }

    return 1;
}

unsigned bp_get_padd( struct bit_pack_data *bpd )
{
    return bpd->ti_.filling_ ? (CHAR_BIT - bpd->ti_.filling_) : 0;
}

size_t bp_get_size( struct bit_pack_data *bpd )
{
    return mem_block_size(bpd->data_) + ((bpd->ti_.filling_ != 0) ? 1 : 0);
}

