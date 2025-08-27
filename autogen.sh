#!/bin/sh -x

rm -fv ltmain.sh config.sub config.guess config.h.in config.rpath
autopoint --force
rm -fv po/Makevars.template po/ChangeLog m4/ChangeLog
aclocal
autoheader
automake --add-missing --copy --force
autoreconf --install
