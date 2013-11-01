TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.c \
    bitpacker/bitpack.c \
    lists/list-work.c \
    trees/aa-tree.c \
    memory/mm-block.c \
    memory/mm-array.c

HEADERS += \
    bitpacker/bitpack.h \
    inc/struct-fields.h \
    lists/list-work.h \
    trees/aa-tree.h \
    memory/mm-block.h \
    memory/mm-array.h


