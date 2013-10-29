TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.c \
    bitpack.c \
    mem-block.c

HEADERS += \
    bitpack.h \
    mem-block.h \
    ../inc/struct-fields.h

