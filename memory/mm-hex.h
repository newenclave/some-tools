#if !defined(MM_HEX_20131116_H)
#define MM_HEX_20131116_H

#include <stdlib.h>

struct mm_block;

void mm_hex_bytes2hex( const void *bytes, size_t length, char *hex );
int  mm_hex_bytes2hex_append( const void *bytes, size_t length,
                                               struct mm_block *hex );
int  mm_hex_bytes2hex_block( const void *bytes, size_t length,
                                        struct mm_block *hex );

size_t mm_hex_hex2bytes( const void *hex, size_t hex_length, void *bytes );


#endif // MM_HEX_20131116_H
