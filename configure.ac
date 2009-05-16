AC_PREREQ(2.54)

AC_INIT([creatures-sprites], [1.3.0])
AC_CONFIG_SRCDIR([src/blkimage.c])
AM_CONFIG_HEADER(config.h)

AC_DEFINE(PLUGIN_NAME, PACKAGE_NAME, [Plug-In name])
AC_DEFINE(PLUGIN_VERSION, PACKAGE_VERSION, [Plug-In version])

AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

AM_MAINTAINER_MODE

# Use -Wall if we have gcc.
if test "$GCC" = "yes"; then
	case "-Wall " in
	"$CFLAGS ") ;;
	*) CFLAGS="$CFLAGS -Wall" ;;
	esac
fi

# GIMP

GIMP_REQUIRED_VERSION=2.2.0

PKG_CHECK_MODULES(GIMP,
  gimp-2.0 >= $GIMP_REQUIRED_VERSION gimpui-2.0 >= $GIMP_REQUIRED_VERSION)

AC_SUBST(GIMP_CFLAGS)
AC_SUBST(GIMP_LIBS)

GIMP_LIBDIR=`$PKG_CONFIG --variable=gimplibdir gimp-2.0`
AC_SUBST(GIMP_LIBDIR)

# libc16

PKG_CHECK_MODULES(C16, c16)
AC_SUBST(C16_CFLAGS)
AC_SUBST(C16_LIBS)

# i18n stuff

GETTEXT_PACKAGE=creatures-sprites
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
				    [The gettext translation domain.])

ALL_LINGUAS="de"

AC_PROG_INTLTOOL
AM_GLIB_GNU_GETTEXT

AC_CHECK_FUNCS(bind_textdomain_codeset)

LOCALEDIR='${datadir}/locale'
DATADIR='${datadir}/plugin_name'

AC_SUBST(LOCALEDIR)
AC_SUBST(DATADIR)

AC_CONFIG_FILES([
Makefile
src/Makefile
po/Makefile.in
])

AC_OUTPUT
