#-----------------------------------------------------------------------------
# Common configuration for all projects.
#-----------------------------------------------------------------------------

CONFIG += c++11

# we don't like warnings...
QMAKE_CXXFLAGS += -Werror
# Disable RTTI
QMAKE_CXXFLAGS += -fno-exceptions -fno-rtti

!defined(TOP_SRC_DIR, var) {
    TOP_SRC_DIR = $$PWD
    TOP_BUILD_DIR = $${TOP_SRC_DIR}/$(BUILD_DIR)
}

include(coverage.pri)
