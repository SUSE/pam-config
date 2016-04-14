#!/bin/sh -x

aclocal
autoheader
automake --add-missing --copy
autoreconf
chmod 755 configure
