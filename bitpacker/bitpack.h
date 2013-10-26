#if !defined(_BITPACK_H_)
#define _BITPACK_H_

#include <stdlib.h>

struct bit_pack_data;

struct bit_pack_data *bp_new_bitpack_data( );
void bp_delete_bitpack_data( struct bit_pack_data *bpd );
void bp_dump( struct bit_pack_data *bpd );

int bp_add_bits(struct bit_pack_data *bpd, unsigned value, unsigned bit_count);
unsigned bp_get_padd( struct bit_pack_data *bpd );
size_t bp_get_size( struct bit_pack_data *bpd );

int bp_is_space_enough( struct bit_pack_data *bpd, unsigned bit_count );

#endif
