#if !defined(CS_UTF8_20131114_H)
#define CS_UTF8_20131114_H

#include <stdint.h>
#include <stdlib.h>

size_t cs_ucs4_to_utf8( uint32_t ucs, char *container, size_t available );
size_t cs_utf8_to_ucs4( const char *utf8, size_t available, u_int32_t *ucs );

#endif
