#if !defined(MM_HEX_20131116_H)
#define MM_HEX_20131116_H

#include <stdlib.h>

struct mm_block;

size_t mm_hex_bytes2hex( const void *bytes, size_t length, char *hex );
size_t mm_hex_bytes2hex_append( const void *bytes, size_t length,
                                                struct mm_block *hex );
size_t mm_hex_bytes2hex_block( const void *bytes, size_t length,
                                                struct mm_block *hex );

size_t mm_hex_hex2bytes( const void *hex, size_t hex_length, void *bytes );
size_t mm_hex_hex2bytes_append( const void *hex, size_t hex_length,
                                                struct mm_block *bytes );
size_t mm_hex_hex2bytes_block( const void *hex, size_t hex_length,
                                                struct mm_block *bytes );


#endif // MM_HEX_20131116_H
