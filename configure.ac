dnl Process this file with autoconf to produce a configure script.
AC_INIT([pam-config],[2.14])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/pam-config.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_PREFIX_DEFAULT(/usr)
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)

if test x"$ac_default_prefix" = x"/usr"
then
        if test ${sysconfdir} = '${prefix}/etc'
        then
                sysconfdir="/etc"
        fi
	if test x"${mandir}" = x'${prefix}/man'
	then
		mandir='${prefix}/share/man'
	fi
fi

EXTRA_CFLAGS="-Werror -W -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -DXTSTRINGDEFINES -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wshadow -Wstrict-prototypes -Wundef"
AC_SUBST(EXTRA_CFLAGS)
dnl Checks for programs.
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl Checks for libraries.
dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST


dnl
dnl Check for xsltproc
dnl
enable_man=yes
AC_PATH_PROG([XSLTPROC], [xsltproc])
if test -z "$XSLTPROC"; then
     enable_man=no
fi
AC_PATH_PROG([XMLLINT], [xmllint],[/bin/true])
dnl check for DocBook DTD and stylesheets in the local catalog.
JH_CHECK_XML_CATALOG([-//OASIS//DTD DocBook XML V4.4//EN],
                [DocBook XML DTD V4.4], [], enable_man=no)
JH_CHECK_XML_CATALOG([http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl],
                [DocBook XSL Stylesheets], [], enable_man=no)
AM_CONDITIONAL(ENABLE_REGENERATE_MAN, test x$enable_man != xno)


dnl internationalization macros
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_REQUIRE_VERSION([0.20.2])

AC_CHECK_FUNCS(dngettext)

AH_VERBATIM([__ZZENABLE_NLS],
[#ifdef ENABLE_NLS
#include <locale.h>
#include <libintl.h>
#define _(msgid) gettext(msgid)
#define N_(msgid) msgid
#else
#define _(msgid) (msgid)
#define N_(msgid) msgid
#endif /* ENABLE_NLS */])

PKG_CHECK_MODULES([INIH], [inih],
  [AC_DEFINE([HAVE_LIBINIH], [1], [Define to 1 if inih is available])],
  [AC_MSG_ERROR([inih library is required])])

dnl Add the variables to the output
AC_SUBST([INIH_CFLAGS])
AC_SUBST([INIH_LIBS])

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in tests/Makefile tests/config/Makefile tests/pam-config.test/Makefile tests/etc/Makefile])
AC_OUTPUT
