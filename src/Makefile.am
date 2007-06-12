#
# Copyright (c) 2006 Thorsten Kukuk <kukuk@thkukuk.de>
#

localedir = $(datadir)/locale

DEFS = @DEFS@ -DLOCALEDIR=\"$(localedir)\" -DCONFDIR=\"$(sysconfdir)\" 

AM_CFLAGS = @EXTRA_CFLAGS@

CLEANFILES = *~

EXTRA_DIST = ${MANS} ${XMLS}

man_MANS = pam-config.8
XMLS = pam-config.8.xml

sbin_PROGRAMS = pam-config

pam_config_SOURCES = pam-config.c load_config.c write_config_session.c \
	write_config_password.c write_config_auth.c write_config_account.c \
	load_obsolete_conf.c

noinst_HEADERS = pam-config.h

if ENABLE_REGENERATE_MAN
%.8: %.8.xml
	$(XMLLINT) --nonet --xinclude --postvalid --noout $<
	$(XSLTPROC) -o $(srcdir)/$@ --path $(srcdir) --xinclude --nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $<
endif

