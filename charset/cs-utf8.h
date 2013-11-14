#if !defined(CS_UTF8_20131114_H)
#define CS_UTF8_20131114_H

#include <stdint.h>
#include <stdlib.h>

int cs_ucs4_to_utf8(uint32_t ucs, char *container, size_t *available);

#endif
