TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.c \
    bitpacker/bitpack.c \
    memory/mem-block.c \
    memory/mem-array.c \
    lists/list-work.c

HEADERS += \
    bitpacker/bitpack.h \
    memory/mem-block.h \
    inc/struct-fields.h \
    memory/mem-array.h \
    lists/list-work.h


