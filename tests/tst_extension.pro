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
    SignOnExtension \
    libqtdbusmock-1 \
    libqtdbustest-1

DEFINES += \
    DBUS_MOCK_TEMPLATE=\\\"$${PWD}/dbus_apparmor.py\\\" \
    PLUGIN_PATH=\\\"../src/libsignon-apparmor-ac.so\\\"

SOURCES = \
    tst_extension.cpp

HEADERS += \
    fake_dbus.h

check.commands = "LD_PRELOAD=mock/libfakeapparmor.so ./$${TARGET}"
check.depends = $${TARGET}
QMAKE_EXTRA_TARGETS += check

QMAKE_CLEAN += $$TARGET
