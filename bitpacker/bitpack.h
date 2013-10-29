#if !defined(BITPACK_20131026_H)
#define BITPACK_20131026_H

#include <stdlib.h>

struct bit_pack_data;

struct bit_pack_data *bit_pack_new( );
void bit_pack_free( struct bit_pack_data *bpd );

int bp_add_bits(struct bit_pack_data *bpd, size_t value, unsigned bit_count);
unsigned bp_get_padd( struct bit_pack_data *bpd );

unsigned bp_get_tail_size( struct bit_pack_data *bpd );
size_t bp_get_size(  struct bit_pack_data *bpd );
size_t bp_copy_data( struct bit_pack_data *bpd, void *to, size_t maximum );

struct bit_unpack_data;

struct bit_unpack_data *bit_unpack_new( );
struct bit_unpack_data *bit_unpack_new2( const void *data, size_t len );
void bit_unpack_free( struct bit_unpack_data *bud );

int      bu_change_size( struct bit_unpack_data *bud, size_t new_len );
unsigned bu_get_bits( struct bit_unpack_data *bud, size_t *val, unsigned len );
int      bu_eod( struct bit_unpack_data *bud );
int      bu_reset( struct bit_unpack_data *bud );
int      bu_assign( struct bit_unpack_data *bud, const void *data, size_t len );

#endif
