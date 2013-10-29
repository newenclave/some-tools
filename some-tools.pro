TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.c \
    bitpacker/bitpack.c \
    memory/mem-block.c

HEADERS += \
    bitpacker/bitpack.h \
    memory/mem-block.h \
    inc/struct-fields.h


