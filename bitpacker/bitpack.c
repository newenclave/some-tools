#include <limits.h>
#include <memory.h>
#include <stdlib.h>

#include <stdio.h>

#include "bitpack.h"
#include "mem-block.h"

typedef size_t          value_type;
typedef unsigned char   container_type;

unsigned char make_char_mask( unsigned count )
{
    static const unsigned char masks[CHAR_BIT + 1] = { // 8 + zero
        0, 1, 3, 7, 15, 31, 63, 127, 255,
#if CHAR_BIT > 8
    #error "wow!";
#endif
    };
    return masks[ count % (CHAR_BIT + 1) ];
}

size_t make_size_mask( unsigned count )
{
    return (unsigned char)(((1 << count) - 1) & 0xFF);
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

    val_ = (val_ << count) | (( dst >> shift ) & make_char_mask( count ));

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

    dst_ |= ((val & make_char_mask( count )) << shift);

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

    if( bpd->data_ != tmp_data ) { // this is new block
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

unsigned bp_get_tail_size( struct bit_pack_data *bpd )
{
    return bpd->ti_.filling_;
}

size_t bp_copy_data( struct bit_pack_data *bpd, void *to, size_t maximum )
{
    size_t data_size = mem_block_size( bpd->data_ );
    void *data = mem_block_data( bpd->data_ );

    if( (maximum == 0) || (data_size == 0) ) {

        return 0;

    } else if( maximum <= data_size ) {

        memcpy( to, data, maximum );
        return maximum;

    } else {
        memcpy( to, data, data_size );
        if( bpd->ti_.filling_ != 0) {
            ((char *)to)[data_size++] = bpd->ti_.current_; // add tail byte
        }
        return data_size;
    }
}
