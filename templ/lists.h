#ifndef LISTS_H
#define LISTS_H

#define list_define_custom_type( type, type_name )              \
  typedef struct type##_list_type {                             \
      type              dat_;                                   \
      type##_list_type *nxt_;                                   \
  } type_name

#define list_define_type( type )            \
    list_define_custom_type( type, type##_list_type )

#define list_init( default_value )    { default_value, NULL }

#endif // LISTS_H
