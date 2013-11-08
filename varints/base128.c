
#include "base128.h"
#include "../memory/mm-block.h"

int b128_unpack_shift( void **data, size_t *lenght, size_t *result )
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

int b128_unpack2( void *data, size_t lenght, size_t *result )
{
    return b128_unpack_shift( &data, &lenght, result );
}

int b128_unpack( struct mm_block *container, size_t *result )
{
    return b128_unpack2(mm_block_begin(container),
                        mm_block_size( container ), result);
}

int b128_pack_shift( size_t number, void **container, size_t *avail )
{
    int   result        = 0;
    unsigned char *data = (unsigned char *)*container;
    size_t tmp          = *avail;

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
            result = (number == 0);
        }
        *container = data;
        *avail     = tmp;
    }
    return result;
}

int b128_pack2( size_t number, void *container, size_t avail )
{
    return b128_pack_shift(number, &container, &avail);
}

int b128_pack_append( size_t number, struct mm_block *container )
{
    int result = 0;
    if( number <= 0x7F ) {
        result = mm_block_push_back( container, (char)(number & 0x7F));
    } else {
        size_t old_size = mm_block_size( container );
        unsigned char next;
        result = 1;
        while( number && result ) {
            next = (unsigned char)(number & 0x7F);
            next |= (( number >>= 7 ) ? 0x80 : 0x00 );
            result = mm_block_push_back( container, next );
        }
        if( !result )
            mm_block_resize( container, old_size );
    }
    return result;
}

int b128_pack( size_t number, struct mm_block *container )
{
    int result = 0;
    struct mm_block *tmp = mm_block_new_reserved(sizeof(number));
    if( tmp ) {
        result = b128_pack_append( number, tmp );
        if( result )
            mm_block_swap( container, tmp );
        mm_block_free( tmp );
    }
    return result;
}
