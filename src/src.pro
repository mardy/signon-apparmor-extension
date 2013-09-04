include(../common-project-config.pri)
include($${TOP_SRC_DIR}/common-vars.pri)
include($${TOP_SRC_DIR}/common-installs-config.pri)

TEMPLATE = lib
TARGET = signon-apparmor-ac

CONFIG += \
    link_pkgconfig \
    plugin \
    qt

QT += \
    core \
    dbus
QT -= gui

QMAKE_CXXFLAGS += \
    -fvisibility=hidden

PKGCONFIG += \
    SignOnExtension \
    dbus-1 \
    libapparmor

# The following use of pkg-config + sed is a hack to workaround a qmake
# limitation: the CFLAGS variable is not used with the moc compiler, so the
# include rules given out by pkg-config wouldn't be applied when running the
# moc.
INCLUDEPATH += . \
    $$system(pkg-config --cflags SignOnExtension | sed s/-I//g)

HEADERS = \
    access-control-manager.h \
    plugin.h

SOURCES = \
    access-control-manager.cpp \
    plugin.cpp

target.path = $$system(pkg-config --variable=plugindir SignOnExtension)
INSTALLS = target

