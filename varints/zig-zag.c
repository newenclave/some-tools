#include "zig-zag.h"


unsigned long zig_zag_fixl(long inp)
{
    return ZIG_ZAG_FIX( long, inp );
}

long zig_zag_unfixl(unsigned long inp)
{
    return ZIG_ZAG_UNFIX( long, inp );
}

unsigned int zig_zag_fix(int inp)
{
    return ZIG_ZAG_FIX( int, inp );
}

int zig_zag_unfix(unsigned int inp)
{
    return ZIG_ZAG_UNFIX( int, inp );
}

unsigned short zig_zag_fixs(short inp)
{
    return ZIG_ZAG_FIX( short, inp );
}

short zig_zag_unfixs(unsigned short inp)
{
    return ZIG_ZAG_UNFIX( short, inp );
}

unsigned char zig_zag_fixc(char inp)
{
    return ZIG_ZAG_FIX( char, inp );
}

char zig_zag_unfixc(unsigned char inp)
{
    return ZIG_ZAG_UNFIX( char, inp );
}
