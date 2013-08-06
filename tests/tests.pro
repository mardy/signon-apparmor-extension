include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)

TARGET = tst_extension

CONFIG += \
    link_pkgconfig \
    qt

QT += \
    core \
    dbus \
    testlib
QT -= gui

PKGCONFIG += \
    SignOnExtension

DEFINES += \
    PLUGIN_PATH=\\\"../src/libsignon-apparmor-ac.so\\\"

HEADERS = \
    tst_extension.h

SOURCES = \
    tst_extension.cpp

check.commands = "./$${TARGET}"
check.depends = $${TARGET}
QMAKE_EXTRA_TARGETS += check

QMAKE_CLEAN += $$TARGET
