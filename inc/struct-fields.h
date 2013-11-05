#if !defined(STRUCTFIELDS_20131029_H)
#define STRUCTFIELDS_20131029_H

/// some nice well-known macros

#define FIELD_OFFSET( type, field ) \
    ((unsigned long)( &((const type *)0)->field ))

#define FIELD_ENTRY( ptr, type, field ) \
    ((type *)((const char *)(ptr) - FIELD_OFFSET(type, field)))


#endif // STRUCTFIELDS_H
