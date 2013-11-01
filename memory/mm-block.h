#if !defined(mm_block_20131026_H)
#define mm_block_20131026_H

#include <stdlib.h>

struct mm_block_data;

struct mm_block_data *mm_block_new( size_t init_size );
struct mm_block_data *mm_block_new_copy( const struct mm_block_data *oth );
void   mm_block_free(struct mm_block_data *mb);

size_t mm_block_size      (struct mm_block_data *mb);
size_t mm_block_capacity  (struct mm_block_data *mb);
size_t mm_block_available (struct mm_block_data *mb);
void*  mm_block_data      (struct mm_block_data *mb);

int mm_block_resize  (struct mm_block_data *mb, size_t new_size);
int mm_block_resize2 (struct mm_block_data *mb, size_t new_size, int c);
int mm_block_reserve (struct mm_block_data *mb, size_t new_size);
int mm_block_clear   (struct mm_block_data *mb);
int mm_block_swap    (struct mm_block_data *lmb, struct mm_block_data *rmb);

int mm_block_concat   (struct mm_block_data *mb,
                         const void *data, size_t len);

int mm_block_concat2  (struct mm_block_data *mb,
                         const struct mm_block_data *cmb);

int mm_block_push_back(struct mm_block_data *mb, char c);

#endif
