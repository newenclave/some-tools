#include <limits.h>
#include <memory.h>
#include <stdlib.h>

#include <stdio.h>

#include "bitpack.h"
#include "../memory/mm-block.h"

typedef size_t          value_type;
typedef unsigned char   container_type;

struct tail_info {
    container_type current_;
    unsigned       filling_;
};

struct bit_pack_data {
    struct mm_block  *data_;
    struct tail_info        ti_;
};

typedef struct bit_pack_data  bit_pack_data_type;


struct head_info {
    const unsigned char *current_;
    unsigned             filling_;
};

struct bit_unpack_data {
    const unsigned char *   data_;
    const unsigned char *   end_;
    size_t                  current_;
    struct head_info        hi_;
};

/*
masks

0x1, 0x3, 0x7, 0xF,
0x1F, 0x3F, 0x7F, 0xFF,

0x1FF, 0x3FF, 0x7FF, 0xFFF,
0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF,

0x1FFFF, 0x3FFFF, 0x7FFFF, 0xFFFFF,
0x1FFFFF, 0x3FFFFF, 0x7FFFFF, 0xFFFFFF,

0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF, 0xFFFFFFF,
0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF,

0x1FFFFFFFF, 0x3FFFFFFFF, 0x7FFFFFFFF, 0xFFFFFFFFF,
0x1FFFFFFFFF, 0x3FFFFFFFFF, 0x7FFFFFFFFF, 0xFFFFFFFFFF,

0x1FFFFFFFFFF, 0x3FFFFFFFFFF, 0x7FFFFFFFFFF, 0xFFFFFFFFFFF,
0x1FFFFFFFFFFF, 0x3FFFFFFFFFFF, 0x7FFFFFFFFFFF, 0xFFFFFFFFFFFF,

0x1FFFFFFFFFFFF, 0x3FFFFFFFFFFFF, 0x7FFFFFFFFFFFF, 0xFFFFFFFFFFFFF,
0x1FFFFFFFFFFFFF, 0x3FFFFFFFFFFFFF, 0x7FFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF,

0x1FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF,
0x1FFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF,
*/

static unsigned char make_char_mask( unsigned count )
{
    static const unsigned char masks[] = { // zero and 8 bits
       0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF
#if CHAR_BIT > 8
    #error "wow!";
#endif
    };
    return masks[ count % (CHAR_BIT + 1) ];
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

bit_pack_data_type *bit_pack_new( )
{
    bit_pack_data_type *new_data = (struct bit_pack_data *)
                                    malloc(sizeof(bit_pack_data_type));

    if( NULL == new_data ) return NULL;

    new_data->data_ = mm_block_new( );

    if( NULL == new_data->data_ ) {
        free( new_data );
        return NULL;
    }

    new_data->ti_.current_ = 0;
    new_data->ti_.filling_ = 0;

    return new_data;
}

void bit_pack_free( bit_pack_data_type *bpd )
{
    if( bpd ) {
        mm_block_free(bpd->data_);
        free( bpd );
    }
}

int bp_add_bits(struct bit_pack_data *bpd, size_t value, unsigned bit_count)
{
    struct tail_info tmp_ti;
    struct mm_block *tmp_data = bpd->data_;
    unsigned tail = bit_count;

    tmp_ti.current_ = bpd->ti_.current_;
    tmp_ti.filling_ = bpd->ti_.filling_;

    if( mm_block_available( bpd->data_ ) <= (bit_count / CHAR_BIT)) {
        tmp_data = mm_block_new( );
        if( NULL == tmp_data ) {
            return 0;
        }
        mm_block_reserve( tmp_data, 8 );
    }

    do {
        tail = pack_bits( value, tail,
                         &tmp_ti.current_, &tmp_ti.filling_ );

        if( tmp_ti.filling_ == CHAR_BIT ) {
            if( 0 == mm_block_push_back(tmp_data, tmp_ti.current_)) {
                if( tmp_data != bpd->data_ )
                    mm_block_free( tmp_data );
                return 0;
            }
            tmp_ti.current_ = 0;
            tmp_ti.filling_ = 0;
        }
    } while ( tail != 0 );

    if( bpd->data_ != tmp_data ) { // this is new block
        int result = 0;
        if( 0 != mm_block_concat2( bpd->data_, tmp_data )) {
            bpd->ti_.current_ = tmp_ti.current_;
            bpd->ti_.filling_ = tmp_ti.filling_;
            result = 1;
        }
        mm_block_free( tmp_data );
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
    return mm_block_size(bpd->data_) + ((bpd->ti_.filling_ != 0) ? 1 : 0);
}

unsigned bp_get_tail_size( struct bit_pack_data *bpd )
{
    return bpd->ti_.filling_;
}

size_t bp_copy_data( struct bit_pack_data *bpd, void *to, size_t maximum )
{
    size_t data_size = mm_block_size( bpd->data_ );
    void *data = mm_block_begin( bpd->data_ );

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

/* ==========  unpack =========== */

struct bit_unpack_data *bit_unpack_new( )
{
    struct bit_unpack_data *new_data =
            (struct bit_unpack_data *)calloc(1, sizeof(struct bit_unpack_data));
    return new_data;
}

struct bit_unpack_data *bit_unpack_new2( const void *data, size_t len )
{
    struct bit_unpack_data *new_data = bit_unpack_new( );
    if(  NULL != new_data ) {
        bu_assign( new_data, data, len );
    }
    return new_data;
}


int bu_reset( struct bit_unpack_data *bud )
{
    bud->hi_.current_ = bud->data_;
    bud->hi_.filling_ = 0;
    return 1;
}

int bu_assign( struct bit_unpack_data *bud, const void *data, size_t len )
{
    bud->data_ = (unsigned char *)(data);
    if( (len > 0) && (NULL != data)) {
        bud->end_ = (const unsigned char *)(data) + len;
    } else {
        bud->end_ = NULL;
    }

    bud->hi_.current_ = bud->data_;
    bud->hi_.filling_ = 0;

    return 1;
}


void bit_unpack_free( struct bit_unpack_data *bud )
{
    if( NULL != bud ) {
        free( bud );
    }
}

int bu_change_size( struct bit_unpack_data *bud, size_t new_len )
{
    if( NULL == bud->data_ ) {
        return 0;
    }
    bud->end_ = bud->data_ + new_len;
    if( bud->hi_.current_ >= bud->end_ ) {
        bud->hi_.current_ = bud->end_;
        bud->hi_.filling_ = 0;
    }
    return 1;
}

int bu_eod( struct bit_unpack_data *bud )
{
    return (bud->hi_.current_ >= bud->end_);
}


unsigned bu_get_bits( struct bit_unpack_data *bud, size_t *val, unsigned len )
{
    unsigned tail = 0;
    size_t value = 0;

    if( bud->hi_.current_ == bud->end_ ) {
        return 0;
    }

    tail = len;
    do {
        tail = unpack_bits(&value, tail,
                           *bud->hi_.current_, &bud->hi_.filling_);
        if( bud->hi_.filling_ == CHAR_BIT ) {
            ++bud->hi_.current_;
            bud->hi_.filling_ = 0;
        }
    } while ( tail && (bud->hi_.current_ != bud->end_) );

    *val = value;
    return len - tail;
}

