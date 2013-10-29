#ifndef STRUCTFIELDS_20131029_H
#define STRUCTFIELDS_20131029_H

/// some nice well-known macros

#define field_offset( type, field ) \
    ((unsigned long)( &((const type *)0)->field ))

#define field_entry( ptr, type, field ) \
    ((type *)((const char *)(ptr) - field_offset(type, field)))


#endif // STRUCTFIELDS_H
