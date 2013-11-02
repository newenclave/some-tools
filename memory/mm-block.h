#if !defined(MM_BLOCK_20131026_H)
#define MM_BLOCK_20131026_H

#include <stdlib.h>

struct mm_block;

#ifdef MM_BLOCK_USE_CUSTOM_ALLOC
void * (* mm_general_alloc)(size_t size);
void   (* mm_general_free) (void *ptr);
#endif

struct mm_block *mm_block_new( size_t init_size );
struct mm_block *mm_block_new_reserved( size_t reserve_size );
struct mm_block *mm_block_new_copy( const struct mm_block *oth );
void   mm_block_free(struct mm_block *mb);

size_t mm_block_size      (struct mm_block *mb);
size_t mm_block_capacity  (struct mm_block *mb);
size_t mm_block_available (struct mm_block *mb);
void*  mm_block_data      (struct mm_block *mb);

int mm_block_resize  (struct mm_block *mb, size_t new_size);
int mm_block_resize2 (struct mm_block *mb, size_t new_size, int c);
int mm_block_reserve (struct mm_block *mb, size_t new_size);
int mm_block_clear   (struct mm_block *mb);
int mm_block_swap    (struct mm_block *lmb, struct mm_block *rmb);

int mm_block_concat   (struct mm_block *mb,
                         const void *data, size_t len);

int mm_block_concat2  (struct mm_block *mb,
                         const struct mm_block *cmb);

int    mm_block_push_back( struct mm_block *mb, char c);

void  *mm_block_create_back( struct mm_block *mb, size_t count );
void  *mm_block_create_front( struct mm_block *mb, size_t count );
void  *mm_block_create_insertion( struct mm_block *mb,
                                  size_t position, size_t count );


#endif // MM_BLOCK_20131026_H
