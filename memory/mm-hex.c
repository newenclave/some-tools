
#include <stdint.h>

#include "mm-hex.h"
#include "mm-block.h"

static const char hex_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
};

void mm_hex_bytes2hex( const void *bytes, size_t length, char *hex )
{
    const uint8_t *pb = (const uint8_t *)bytes;
    char *ph                = hex;
    while( length-- ) {
        *ph++ = hex_table[ (*pb >> 4) & 0x0F ];
        *ph++ = hex_table[ *pb++ & 0x0F ];
    }
}

int mm_hex_bytes2hexappend( const void *bytes, size_t length,
                                        struct mm_block *hex )
{
    size_t old_size = mm_block_size( hex );
    int result = mm_block_resize( hex, old_size + (length << 1) );
    if( result ) {
        mm_hex_bytes2hex( bytes, length, (char *)mm_block_at( hex, old_size ));
    }
    return result;
}


int mm_hex_bytes2hexblock( const void *bytes, size_t length,
                                        struct mm_block *hex )
{
    int result = 0;
    struct mm_block *new_block = mm_block_new_reserved( length << 1 );
    if( new_block ) {
        result = mm_hex_bytes2hexappend( bytes, length, new_block );
        if( result ) {
            mm_block_swap( hex, new_block );
        }
        mm_block_free(new_block);
    }
    return result;
}
