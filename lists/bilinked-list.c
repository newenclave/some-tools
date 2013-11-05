#include "bilinked-list.h"

size_t bilinked_list_length( struct bilinked_list_head *bll,
                             enum bilinked_list_direction direct )
{
    size_t res = 0;
    for( ;bll ; bll = BILINKED_LIST_NEXT( bll, direct ), ++res );
    return res;
}

size_t bilinked_list_length_full( struct bilinked_list_head *bll )
{
    size_t res = bilinked_list_length( bll, BILINK_DIRECT_FORWARD )
               + bilinked_list_length( bll, BILINK_DIRECT_BACKWARD );

    return res ? res - 1 : res;
}

