#ifndef ZIG_ZAG_20131108_H
#define ZIG_ZAG_20131108_H

/*
 * zig-zag:
 *  -3 -> 5
 *  -2 -> 3
 *  -1 -> 1
 *   0 -> 0
 *   1 -> 2
 *   2 -> 4
 *   3 -> 6
 *
 *   for signed 32 bit values (val << 1) ^ (val >> 31)
 *   for signed 64 bit values (val << 1) ^ (val >> 63)
 *
 *  but we use unsigned values
*/

#define ZIG_ZAG_UNFIX( type_name, inp )             \
        ((type_name)(((inp) & 1) ? (~((inp) >> 1)) : ((inp) >> 1)))

#define ZIG_ZAG_TOP_BIT_SHIFT(type_name) ((sizeof(type_name) << 3 ) -1 )

#define ZIG_ZAG_TOP_BIT( type_name, inp )           \
    ((unsigned type_name)(inp) >> ZIG_ZAG_TOP_BIT_SHIFT(type_name))

#define ZIG_ZAG_FIX( type_name, inp )               \
   ((ZIG_ZAG_TOP_BIT(type_name, inp)        ?       \
        ((~(unsigned type_name)(inp)) << 1) :       \
          ((unsigned type_name)(inp) << 1)) ^       \
     ZIG_ZAG_TOP_BIT(type_name, inp))


unsigned long zig_zag_fixl(long inp);
long          zig_zag_unfixl(unsigned long inp);

unsigned int zig_zag_fix(int inp);
int          zig_zag_unfix(unsigned int inp);

unsigned int zig_zag_fix(int inp);
int          zig_zag_unfix(unsigned int inp);

unsigned short zig_zag_fixs(short inp);
short          zig_zag_unfixs(unsigned short inp);

unsigned char zig_zag_fixc(char inp);
char          zig_zag_unfixc(unsigned char inp);


#endif
