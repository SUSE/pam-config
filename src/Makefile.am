#
# Copyright (c) 2006, 2008 Thorsten Kukuk <kukuk@thkukuk.de>
#

localedir = $(datadir)/locale

DEFS = @DEFS@ -DLOCALEDIR=\"$(localedir)\" -DCONFDIR=\"$(sysconfdir)\"

AM_CFLAGS = @EXTRA_CFLAGS@

CLEANFILES = *~

EXTRA_DIST = ${MANS} ${XMLS} pam-config.8.xml-header pam-config.8.xml-footer

man_MANS = pam-config.8
XMLS = pam-config.8.xml

sbin_PROGRAMS = pam-config

pam_config_SOURCES = pam-config.c load_config.c write_config.c \
	load_obsolete_conf.c sanity_checks.c pam-module.c \
	supported-modules.h option_set.h option_set.c \
	mod_pam_unix2.c mod_pam_pwcheck.c mod_pam_umask.c mod_pam_ldap.c \
	mod_pam_mkhomedir.c mod_pam_ccreds.c mod_pam_winbind.c \
	mod_pam_localuser.c mod_pam_env.c mod_pam_limits.c mod_pam_krb5.c \
	mod_pam_cracklib.c mod_pam_apparmor.c mod_pam_nam.c mod_pam_pkcs11.c \
	mod_pam_make.c mod_pam_unix.c mod_pam_mount.c mod_pam_deny.c \
	single_config.c mod_pam_loginuid.c mod_pam_lastlog.c \
	mod_pam_cryptpass.c mod_pam_ck_connector.c mod_pam_thinkfinger.c \
	mod_pam_group.c mod_pam_time.c mod_pam_ssh.c mod_pam_succeed_if.c \
	mod_pam_csync.c mod_pam_fp.c mod_pam_fprint.c mod_pam_pwhistory.c \
	mod_pam_selinux.c mod_pam_gnome_keyring.c mod_pam_passwdqc.c \
	mod_pam_exec.c mod_pam_sss.c mod_pam_fprintd.c mod_pam_systemd.c \
	mod_pam_systemd_home.c \
	mod_pam_ecryptfs.c mod_pam_access.c mod_pam_google_authenticator.c \
	mod_pam_kwallet5.c mod_pam_keyinit.c mod_pam_mktemp.c \
	mod_pam_pwquality.c mod_pam_u2f.c mod_pam_faildelay.c \
	mod_pam_lastlog2.c mod_pam_fscrypt.c

noinst_HEADERS = pam-config.h pam-module.h

if ENABLE_REGENERATE_MAN
pam-config.8.xml: pam-config pam-config.8.xml-header pam-config.8.xml-footer
	cp $(srcdir)/pam-config.8.xml-header pam-config.8.xml
	chmod 644 pam-config.8.xml
	$(builddir)/pam-config --xmloutput >> pam-config.8.xml
	cat $(srcdir)/pam-config.8.xml-footer >> pam-config.8.xml

pam-config.8: pam-config.8.xml
	$(XMLLINT) --nonet --xinclude --postvalid --noout $<
	$(XSLTPROC) -o $(builddir)/$@ --path $(builddir) --xinclude --nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $<
endif
