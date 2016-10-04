include(../../common-project-config.pri)

TARGET = fakeapparmor
TEMPLATE = lib

CONFIG += \
    debug \
    link_pkgconfig \
    qt

QT += \
    core

PKGCONFIG += \
    libapparmor

# Error on undefined symbols
QMAKE_LFLAGS += $$QMAKE_LFLAGS_NOUNDEF

SOURCES += \
    apparmor.cpp
