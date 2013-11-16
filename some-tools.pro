TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.c \
    bitpacker/bitpack.c \
    lists/linked-list.c \
    trees/aa-tree.c \
    memory/mm-block.c \
    memory/mm-array.c \
    containers/cnt-deque.c \
    lists/bilinked-list.c \
    varints/base128.c \
    trees/prefix-tree.c \
    varints/zig-zag.c \
    charset/cs-utf8.c \
    memory/mm-hex.c

HEADERS += \
    bitpacker/bitpack.h \
    inc/struct-fields.h \
    lists/linked-list.h \
    trees/aa-tree.h \
    memory/mm-block.h \
    memory/mm-array.h \
    memory/mm-hex.h   \
    containers/cnt-deque.h \
    lists/bilinked-list.h \
    varints/base128.h \
    trees/prefix-tree.h \
    varints/zig-zag.h \
    charset/cs-utf8.h


