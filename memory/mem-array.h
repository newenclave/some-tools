#if !defined(MEMARRAY_20132910_H)
#define MEMARRAY_20132910_H

struct mem_array_data;


struct mem_array_data *mem_array_new( size_t element_size );
struct mem_array_data *mem_array_new2( size_t count, size_t element_size );

#endif // MEMARRAY_H
