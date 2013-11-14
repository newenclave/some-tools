#include "cs-utf8.h"


int cs_ucs4_to_utf8(uint32_t ucs, char *container, size_t *available)
{
    static const u_int32_t utf8_steps[6][2] = {
        { 0x0000000, 0x0000007F },
        { 0x0000080, 0x000007FF },
        { 0x0000800, 0x0000FFFF },
        { 0x0010000, 0x001FFFFF },
        { 0x0200000, 0x03FFFFFF },
        { 0x4000000, 0x7FFFFFFF }
    };

    char *utf8_enc = container;
    size_t used  = 0;
    size_t avail = *available;

    if( avail>=1 && ucs <= 0x7F ) {
        used = 1;
        utf8_enc[0] = (char)(ucs);

    } else if((avail>=2) && (ucs>=utf8_steps[1][0]) && (ucs<=utf8_steps[1][1])){
        used = 2;
        utf8_enc[0] = (char)(0xC0 + ( ucs / 0x40));
        utf8_enc[1] = (char)(0x80 + ( ucs % 0x40));

    } else if((avail>=3) && (ucs>=utf8_steps[2][0]) && (ucs<=utf8_steps[2][1])){
        used = 3;
        utf8_enc[0] = (char)(0xE0 + ( ucs / 0x1000));
        utf8_enc[1] = (char)(0x80 + ((ucs / 0x0040) % 0x40));
        utf8_enc[2] = (char)(0x80 + ( ucs % 0x0040));

    } else if((avail>=4) && (ucs>=utf8_steps[3][0]) && (ucs<=utf8_steps[3][1])){
        used = 4;
        utf8_enc[0] = (char)(0xF0 + ( ucs / 0x40000));
        utf8_enc[1] = (char)(0x80 + ((ucs / 0x01000) % 0x40));
        utf8_enc[2] = (char)(0x80 + ((ucs / 0x00040) % 0x40));
        utf8_enc[3] = (char)(0x80 + ( ucs % 0x00040));

    } else if((avail>=5) && (ucs>=utf8_steps[4][0]) && (ucs<=utf8_steps[4][1])){
        used = 5;
        utf8_enc[0] = (char)(0xF8 + ( ucs / 0x1000000));
        utf8_enc[1] = (char)(0x80 + ((ucs / 0x0040000) % 0x40));
        utf8_enc[2] = (char)(0x80 + ((ucs / 0x0001000) % 0x40));
        utf8_enc[3] = (char)(0x80 + ((ucs / 0x0000040) % 0x40));
        utf8_enc[4] = (char)(0x80 + ( ucs % 0x0000040));

    } else if((avail>=6) && (ucs>=utf8_steps[5][0]) && (ucs<=utf8_steps[6][1])){
        used = 6;
        utf8_enc[0] = (char)(0xFC + ( ucs / 0x40000000));
        utf8_enc[1] = (char)(0x80 + ((ucs / 0x01000000) % 0x40));
        utf8_enc[2] = (char)(0x80 + ((ucs / 0x00040000) % 0x40));
        utf8_enc[3] = (char)(0x80 + ((ucs / 0x00001000) % 0x40));
        utf8_enc[4] = (char)(0x80 + ((ucs / 0x00000040) % 0x40));
        utf8_enc[5] = (char)(0x80 + ( ucs % 0x00000040));
    }

    *available = (avail - used);
    return (used != 0);
}
