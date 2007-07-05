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

pam_config_SOURCES = pam-config.c load_config.c write_config.c \
	load_obsolete_conf.c sanity_checks.c pam-module.h \
	pam-module.c supported-modules.h option_set.h option_set.c \
	mod_pam_unix2.c mod_pam_pwcheck.c mod_pam_umask.c mod_pam_ldap.c \
	mod_pam_mkhomedir.c mod_pam_ccreds.c mod_pam_winbind.c \
	mod_pam_localuser.c mod_pam_env.c mod_pam_limits.c mod_pam_krb5.c \
	mod_pam_cracklib.c mod_pam_apparmor.c mod_pam_nam.c mod_pam_pkcs11.c \
	mod_pam_make.c mod_pam_unix.c mod_pam_mount.c \
	single_config.c mod_pam_loginuid.c mod_pam_lastlog.c

noinst_HEADERS = pam-config.h

if ENABLE_REGENERATE_MAN
%.8: %.8.xml
	$(XMLLINT) --nonet --xinclude --postvalid --noout $<
	$(XSLTPROC) -o $(srcdir)/$@ --path $(srcdir) --xinclude --nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $<
endif

