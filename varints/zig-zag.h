#ifndef ZIG_ZAG_20131108_H
#define ZIG_ZAG_20131108_H

#include <stdlib.h>

#define ZIG_ZAG_UNFIX( type_name, inp )             \
        ((type_name)(((inp) & 1) ? (~((inp) >> 1)) : ((inp) >> 1)))

#define ZIG_ZAG_TOP_BIT_SHIFT( type_name ) ((sizeof(type_name) << 3 ) -1 )

#define ZIG_ZAG_TOP_BIT( type_name, inp )           \
    ((unsigned type_name)(inp) >> ZIG_ZAG_TOP_BIT_SHIFT(type_name))

#define ZIG_ZAG_FIX( type_name, inp )               \
    ((ZIG_ZAG_TOP_BIT(type_name, inp)        ?      \
         ((~(unsigned type_name)(inp)) << 1) :      \
           ((unsigned type_name)(inp) << 1)) ^      \
              ZIG_ZAG_TOP_BIT(type_name, inp))

unsigned int zig_zag_fix(int inp);
         int zig_zag_unfix(unsigned int inp);

unsigned long zig_zag_fixl(long inp);
         long zig_zag_unfixl(unsigned long inp);

unsigned short zig_zag_fixs(short inp);
         short zig_zag_unfixs(unsigned short inp);


#endif
