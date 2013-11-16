#if !defined(MM_HEX_20131116_H)
#define MM_HEX_20131116_H

#include <stdlib.h>

struct mm_block;

void mm_hex_bytes2hex( const void *bytes, size_t length, char *hex );
int  mm_hex_bytes2hexappend( const void *bytes, size_t length,
                                               struct mm_block *hex );
int  mm_hex_bytes2hexblock( const void *bytes, size_t length,
                                        struct mm_block *hex );


#endif // MM_HEX_20131116_H
