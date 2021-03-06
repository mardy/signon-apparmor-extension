include(common-vars.pri)

TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
    src \
    tests

DISTNAME = $${PROJECT_NAME}-$${PROJECT_VERSION}
EXCLUDES = \
    --exclude-vcs \
    --exclude=.* \
    --exclude=$${DISTNAME}.tar.bz2
dist.commands = "tar -cvjf $${DISTNAME}.tar.bz2 $$EXCLUDES --transform='s,^,$$DISTNAME/,' *"
dist.depends = distclean
QMAKE_EXTRA_TARGETS += dist
