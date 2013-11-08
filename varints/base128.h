#ifndef BASE128_20131108_H
#define BASE128_20131108_H

#include <stdlib.h>

struct mm_block;

size_t b128_unpack( void *data, size_t lenght );
void   b128_pack( size_t number, struct mm_block *container );

#endif // BASE128_H
