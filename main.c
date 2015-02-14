
#include <stdio.h>
#include "templ/arrays.h"

array_define_type(size_t);

#define at_index(x, y, m) ((x) * (m) + (y))

void show_array( const size_t_array_type *arr, size_t n, size_t m )
{
    const size_t *ar = &array_at( *arr, 0 );

    size_t i, j;
    for( i=0; i<n; i++ ) {
        for( j=0; j<m; j++ ) {
            printf( "%3lu ", ar[at_index(i, j, m)] );
        }
        printf( "\n" );
    }
}

void fill_array( size_t_array_type *arr,
                 const char *s1, const char *s2,
                 size_t n, size_t m )
{
    size_t i, j;

    size_t *ar = &array_at( *arr, 0 );

    for( i=1; i<n; i++ ) {
        for( j=1; j<m; j++ ) {
            const size_t index = at_index( i, j, m );
            if( s1[i-1] == s2[j-1] ) {
                ar[index] = ar[at_index(i-1, j-1, m)] + 1;
            } else if( ar[at_index(i, j-1, m)] > ar[at_index(i-1, j, m)] ) {
                ar[index] = ar[at_index(i, j-1, m)];
            } else {
                ar[index] = ar[at_index(i-1, j, m)];
            }
        }
    }
}

int main( )
{
    const char *DNA1 = "GCATGCCCAATTGCCGGAATTAGGCTTAG";
    const char *DNA2 = "GAATTAGGATAGGCCCAATTGCCTTGCCG";

    const size_t L1 = strlen( DNA1 ) + 1;
    const size_t L2 = strlen( DNA2 ) + 1;

    int err;

    size_t i;

    size_t_array_type arr = array_init;

    array_resize( arr, L1 * L2, err );
    array_foreach( arr, i ) {
        array_at( arr, i ) = 0;
    }

    fill_array( &arr, DNA1, DNA2, L1, L2 );

    show_array( &arr, L1, L2 );

    array_free( arr );

    return 0;
}
