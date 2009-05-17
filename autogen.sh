#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

olddir=`pwd`
cd $srcdir

PROJECT="creatures-sprites"

#(glib-gettextize --version) < /dev/null > /dev/null 2>&1 || {
#	echo
#	echo "You must have glib-gettextize installed to compile $PROJECT."
#	echo "Install the appropriate package for your distribution,"
#	echo "or get the source tarball at ftp://ftp.gnome.org/pub/GNOME/sources/glib"
#	DIE=1
#}

(intltoolize --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have intltool installed to compile $PROJECT."
	echo "Install the appropriate package for your distribution,"
	echo "or get the source tarball at http://freedesktop.org/wiki/Software/intltool"
	DIE=1
}

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile $PROJECT."
	echo "Install the appropriate package for your distribution,"
	echo "or get the source tarball at http://ftp.gnu.org/gnu/autoconf/"
	DIE=1
}

if test "$DIE" = "1"; then
	echo
	echo "Please fix these issues and then rerun ./autogen.sh"
	exit 1
fi

#glib-gettextize --force || exit $?
intltoolize --force --automake || exit $?

# Redefine ACLOCAL so that it will find shave.m4
ACLOCAL="aclocal -I." \
autoreconf --force --install --symlink --verbose || exit $?

cd $olddir

if test x$NOCONFIGURE = x; then
  echo Running $srcdir/configure $conf_flags "$@" ...
  $srcdir/configure $conf_flags "$@" \
  && echo Now type \`make\' to compile. || exit 1
else
  echo Skipping configure process.
fi

