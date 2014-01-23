#include "zig-zag.h"

unsigned long long zig_zag_fixll(long long inp)
{
    return ZIG_ZAG_FIX( unsigned long long, inp );
}

long long zig_zag_unfixll(unsigned long long inp)
{
    return ZIG_ZAG_UNFIX( long long, inp );
}

unsigned long zig_zag_fixl(long inp)
{
    return ZIG_ZAG_FIX( unsigned long, inp );
}

long zig_zag_unfixl(unsigned long inp)
{
    return ZIG_ZAG_UNFIX( long, inp );
}

unsigned int zig_zag_fix(int inp)
{
    return ZIG_ZAG_FIX( unsigned int, inp );
}

int zig_zag_unfix(unsigned int inp)
{
    return ZIG_ZAG_UNFIX( int, inp );
}

unsigned short zig_zag_fixs(short inp)
{
    return ZIG_ZAG_FIX( unsigned short, inp );
}

short zig_zag_unfixs(unsigned short inp)
{
    return ZIG_ZAG_UNFIX( short, inp );
}

unsigned char zig_zag_fixc(char inp)
{
    return ZIG_ZAG_FIX( unsigned char, inp );
}

char zig_zag_unfixc(unsigned char inp)
{
    return ZIG_ZAG_UNFIX( char, inp );
}
