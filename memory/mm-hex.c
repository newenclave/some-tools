
#include <stdint.h>

#include "mm-hex.h"
#include "mm-block.h"

static const char hex_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
};

size_t mm_hex_bytes2hex( const void *bytes, size_t length, char *hex )
{
    const uint8_t *pb = (const uint8_t *)bytes;
    char *ph          = hex;
    while( length-- ) {
        *ph++ = hex_table[ *pb >> 4 ];
        *ph++ = hex_table[ *pb++ & 0x0F ];
    }
    return (length << 1);
}

size_t mm_hex_bytes2hex_append( const void *bytes, size_t length,
                                        struct mm_block *hex )
{
    size_t old_size = mm_block_size( hex );
    int result = mm_block_resize( hex, old_size + (length << 1) );
    if( result ) {
        mm_hex_bytes2hex( bytes, length, (char *)mm_block_at( hex, old_size ));
    }
    return result ? (length << 1) : 0;
}


size_t mm_hex_bytes2hex_block( const void *bytes, size_t length,
                                        struct mm_block *hex )
{
    struct mm_block *new_block = mm_block_new2( length << 1 );
    if( new_block ) {
        mm_hex_bytes2hex( bytes, length, (char *)mm_block_begin( new_block ));
        mm_block_swap( hex, new_block );
        mm_block_free(new_block);
    }
    return (new_block != NULL) ? (length << 1) : 0;
}

#define MM_HEX_SWITCH_SET_HEX_CODE( hc )\
    switch( hc ) {                      \
    case '0':   case '1':               \
    case '2':   case '3':               \
    case '4':   case '5':               \
    case '6':   case '7':               \
    case '8':   case '9':               \
            hc -= '0';                  \
            break;                      \
    case 'a':   case 'b':               \
    case 'c':   case 'd':               \
    case 'e':   case 'f':               \
            hc = (hc - 'a' + 0xa);      \
            break;                      \
    case 'A':   case 'B':               \
    case 'C':   case 'D':               \
    case 'E':   case 'F':               \
            hc = (hc - 'A' + 0xA);      \
            break;                      \
    default:                            \
        hc = 0xFF;                      \
    }

size_t mm_hex_hex2bytes( const void *hex, size_t hex_length, void *bytes )
{
    const char *ph = (const char *)hex;
    uint8_t    *pb = (uint8_t    *)bytes;

    hex_length >>= 1;
    while( hex_length-- ) {
        uint8_t f = *ph++;
        uint8_t l = *ph++;
        MM_HEX_SWITCH_SET_HEX_CODE( f );
        MM_HEX_SWITCH_SET_HEX_CODE( l );
        if( (f | l) & 0xF0 ) {
            hex_length = 0;
        } else {
            *pb++ = (f << 4 | l);
        }
    }
    return (pb - (uint8_t *)bytes);
}


size_t mm_hex_hex2bytes_append( const void *hex, size_t hex_length,
                                               struct mm_block *bytes )
{
    size_t old_size = mm_block_size( bytes );
    size_t new_size = old_size + (hex_length >> 1);
    size_t new_fill = 0;
    if( mm_block_resize( bytes, new_size ) ) {
        new_fill = mm_hex_hex2bytes( hex, hex_length,
                                     (char *)mm_block_at( bytes, old_size ));
        mm_block_reduce( bytes, new_size - old_size - new_fill );
    }
    return new_fill;
}

size_t mm_hex_hex2bytes_block( const void *hex, size_t hex_length,
                                                struct mm_block *bytes )
{
    struct mm_block *new_block = mm_block_new2( hex_length >> 1 );
    size_t result = 0;
    if( new_block ) {
        result = mm_hex_hex2bytes( hex, hex_length, mm_block_begin(new_block) );
        mm_block_swap( bytes, new_block );
        mm_block_free(new_block);
    }
    return result;
}

