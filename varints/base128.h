#ifndef BASE128_20131108_H
#define BASE128_20131108_H

#include <stdlib.h>
#include <stdint.h>

struct mm_block;

int b128_unpack( const struct mm_block *container, size_t *result );
int b128_unpack2( const void *data, size_t lenght, size_t *result );
int b128_unpack_shift( const void **data, size_t *lenght, size_t *result );

size_t b128_pack( size_t number, struct mm_block *container );
size_t b128_pack2( size_t number, void *container, size_t avail );
size_t b128_pack_shift( size_t number, void **container, size_t *avail );

size_t b128_pack_append( size_t number, struct mm_block *container );

size_t b128_pack_signed_32( int32_t number, void *container, size_t avail );


#endif // BASE128_H
