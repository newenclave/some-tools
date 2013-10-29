TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.c \
    bitpacker/bitpack.c \
    bitpacker/mem-block.c

HEADERS += \
    bitpacker/bitpack.h \
    bitpacker/mem-block.h \
    inc/struct-fields.h


