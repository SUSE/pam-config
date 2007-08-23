#
# Copyright (c) 2006 Thorsten Kukuk <kukuk@thkukuk.de>
#
AUTOMAKE_OPTIONS = 1.7 gnits dist-bzip2
#
SUBDIRS = po src tests

CLEANFILES = *~

M4_FILES = $(shell ls $(srcdir)/m4/*.m4)

EXTRA_DIST = config.rpath  $(M4_FILES)

ACLOCAL_AMFLAGS = -I m4

