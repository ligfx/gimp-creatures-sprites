## Process this file with automake to produce Makefile.in

VERSION_FLAGS = -DVERSION=\"@PLUGIN_VERSION@\"

bin_PROGRAMS = creatures-sprites

bindir = $(GIMP_PLUGIN_DIR)/plug-ins

creatures_sprites_SOURCES = \
	plugin-intl.h	\
	main.c		\
	main.h		

INCLUDES = \
	-I$(top_srcdir)/intl \
	@GIMP_CFLAGS@
	$(VERSION_FLAGS)

LDADD = \
	$(GIMP_LIBS)	\
	$(INTLLIBS) \
	-lc16

AM_CPPFLAGS = \
	-DLOCALEDIR=\""$(LOCALEDIR)"\"		\
	-DDATADIR=\""$(DATADIR)"\"		\
	-DPLUGIN_NAME=\""$(PLUGIN_NAME)"\"

.PHONY: files

files:
	@files=`ls $(DISTFILES) 2> /dev/null`; for p in $$files; do \
	  echo $$p; \
	done
