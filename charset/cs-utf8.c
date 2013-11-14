#include "cs-utf8.h"

static const unsigned char size_table[ ] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0
};

static const u_int32_t step_table[6][2] = {
    { 0x0000000, 0x0000007F },
    { 0x0000080, 0x000007FF },
    { 0x0000800, 0x0000FFFF },
    { 0x0010000, 0x001FFFFF },
    { 0x0200000, 0x03FFFFFF },
    { 0x4000000, 0x7FFFFFFF }
};

size_t cs_ucs4_to_utf8(uint32_t ucs, char *container, size_t avail)
{

    char *utf8_enc = container;
    size_t used  = 0;

    if( avail>=1 && ucs <= 0x7F ) {
        used = 1;
        utf8_enc[0] = (char)(ucs);

    } else if((avail>=2) && (ucs>=step_table[1][0]) && (ucs<=step_table[1][1])){
        used = 2;
        utf8_enc[0] = (char)(0xC0 + ( ucs >>  6 ));
        utf8_enc[1] = (char)(0x80 + ( ucs & 0x3F ));

    } else if((avail>=3) && (ucs>=step_table[2][0]) && (ucs<=step_table[2][1])){
        used = 3;
        utf8_enc[0] = (char)(0xE0 + ( ucs >> 12 ));
        utf8_enc[1] = (char)(0x80 + ((ucs >>  6 ) % 0x40));
        utf8_enc[2] = (char)(0x80 + ( ucs % 0x0040));

    } else if((avail>=4) && (ucs>=step_table[3][0]) && (ucs<=step_table[3][1])){
        used = 4;
        utf8_enc[0] = (char)(0xF0 + ( ucs >> 18 ));
        utf8_enc[1] = (char)(0x80 + ((ucs >> 12 ) & 0x3F));
        utf8_enc[2] = (char)(0x80 + ((ucs >>  6 ) & 0x3F));
        utf8_enc[3] = (char)(0x80 + ( ucs & 0x3F));

    } else if((avail>=5) && (ucs>=step_table[4][0]) && (ucs<=step_table[4][1])){
        used = 5;
        utf8_enc[0] = (char)(0xF8 + ( ucs >> 24 ));
        utf8_enc[1] = (char)(0x80 + ((ucs >> 18 ) & 0x3F));
        utf8_enc[2] = (char)(0x80 + ((ucs >> 12 ) & 0x3F));
        utf8_enc[3] = (char)(0x80 + ((ucs >>  6 ) & 0x3F));
        utf8_enc[4] = (char)(0x80 + ( ucs & 0x3F));

    } else if((avail>=6) && (ucs>=step_table[5][0]) && (ucs<=step_table[6][1])){
        used = 6;
        utf8_enc[0] = (char)(0xFC + ( ucs >> 30 ));
        utf8_enc[1] = (char)(0x80 + ((ucs >> 24 ) & 0x3F));
        utf8_enc[2] = (char)(0x80 + ((ucs >> 18 ) & 0x3F));
        utf8_enc[3] = (char)(0x80 + ((ucs >> 12 ) & 0x3F));
        utf8_enc[4] = (char)(0x80 + ((ucs >>  6 ) & 0x3F));
        utf8_enc[5] = (char)(0x80 + ( ucs & 0x3F));
    }

    return used;
}


size_t cs_utf8_to_ucs4( const char *utf8, size_t available, u_int32_t *ucs )
{

    unsigned char a = utf8[0];
    unsigned char b, c, d, e, f;

    u_int32_t res = 0;
    size_t   used = size_table[a];

    if( available < used ) {
        return 0;
    }

    switch( used ) {
    case 1:
        res = utf8[0];
        break;
    case 2:
        b = utf8[1];
        res = ( (a-0xC0) <<  6 ) +
                (b-0x80);
        break;
    case 3:
        b = utf8[1];
        c = utf8[2];

        res = ( (a-0xE0) << 12 ) +
              ( (b-0x80) <<  6 ) +
                (c-0x80);
        break;
    case 4:
        b = utf8[1];
        c = utf8[2];
        d = utf8[3];

        res = ( (a-0xF0) << 18 ) +
              ( (b-0x80) << 12 ) +
              ( (c-0x80) <<  6 ) +
                (d-0x80);
        break;
    case 5:
        b = utf8[1];
        c = utf8[2];
        d = utf8[3];
        e = utf8[4];

        res =  ( (a-0xF8) << 24 ) +
               ( (b-0x80) << 18 ) +
               ( (c-0x80) << 12 ) +
               ( (d-0x80) <<  6 ) +
                 (e-0x80);
        break;
    case 6:
        b = utf8[1];
        c = utf8[2];
        d = utf8[3];
        e = utf8[4];
        f = utf8[5];

        res = ( (a-0xFC) << 30 ) +
              ( (b-0x80) << 24 ) +
              ( (c-0x80) << 18 ) +
              ( (d-0x80) << 12 ) +
              ( (e-0x80) <<  6 ) +
                (f-0x80);
        break;
    }
    *ucs = res;
    return used;
}
