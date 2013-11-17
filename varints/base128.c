
#include "base128.h"

#include "../memory/mm-block.h"

int b128_unpack_shift( const void **data, size_t *lenght, size_t *result )
{
    size_t   tmp     = 0;
    int      ret     = 0;
    unsigned shift   = 0;
    size_t   next    = 0xFF;
    unsigned char *p = (unsigned char*)*data;
    size_t   len     = *lenght;

    if( len ) do {
        next = *p++;
        tmp |= ((next & 0x7F) << shift);
        shift += 7;
    } while ( --len && (next & 0x80) );

    ret = ((next & 0x80) == 0);

    if( result )
        *result = tmp;

    *data   = p;
    *lenght = len;

    return ret;
}

int b128_unpack2( const void *data, size_t lenght, size_t *result )
{
    return b128_unpack_shift( &data, &lenght, result );
}

int b128_unpack( const struct mm_block *container, size_t *result )
{
    return b128_unpack2(mm_block_const_begin(container),
                        mm_block_size (container), result);
}

size_t b128_pack_shift( size_t number, void **container, size_t *available )
{
    size_t result        = 0;
    unsigned char *data  = (unsigned char *)*container;
    size_t tmp           = *available;

    if( tmp ) {
        if( number <= 0x7F ) {
            *data = (unsigned char)(number & 0x7F);
            result = 1;
            data++;
            tmp--;
        } else {
            char next;
            while( number && tmp-- ) {
                next = (unsigned char)(number & 0x7F);
                next |= (( number >>= 7 ) ? 0x80 : 0x00 );
                *data++ = next;
            }
            if( number == 0 )
                result = (*available) - tmp;
        }
        *container = data;
        *available = tmp;
    }
    return result;
}

size_t b128_pack2( size_t number, void *container, size_t avail )
{
    return b128_pack_shift(number, &container, &avail);
}

size_t b128_pack_append( size_t number, struct mm_block *container )
{
    int    result = 0;
    size_t added  = 0;
    if( number <= 0x7F ) {
        result = mm_block_push_back( container, (char)(number & 0x7F));
        added = 1;
    } else {
        unsigned char next;
        result = 1;
        while( number && result ) {
            next = (unsigned char)(number & 0x7F);
            next |= (( number >>= 7 ) ? 0x80 : 0x00 );
            result = mm_block_push_back( container, next );
            added += (result != 0);
        }
        if( !result ) {
            mm_block_reduce( container, added );
            added = 0;
        }
    }
    return added;
}

size_t b128_pack( size_t number, struct mm_block *container )
{
    size_t result = 0;
    struct mm_block *tmp = mm_block_new_reserved(sizeof(number));
    if( tmp ) {
        result = b128_pack_append( number, tmp );
        if( result )
            mm_block_swap( container, tmp );
        mm_block_free( tmp );
    }
    return result;
}


size_t b128_pack_signed_32( int32_t number, void *container, size_t avail )
{
    size_t tmp = avail;
    if( avail ) {
        uint8_t *data = (uint8_t *)container;
        uint32_t unumber = (uint32_t)number;
        uint8_t signbit = ((unumber & (1 << 31)) != 0);
        uint8_t next = 0;

        unumber = signbit ? (~unumber + 1) : unumber;
        next = (uint8_t)(unumber & 0x3F) | (signbit << 6);

        unumber >>= 6;
        *data = next;
        tmp--;
        while (unumber && tmp--) {
            next = (uint8_t)(unumber & 0x7F);
            next |= (( unumber >>= 7 ) ? 0x80 : 0x00 );
            *data++ = next;
        }
    }
    return avail - tmp;
}
