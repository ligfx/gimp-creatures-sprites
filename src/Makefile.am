## Process this file with automake to produce Makefile.in

VERSION_FLAGS = -DVERSION=\"@PLUGIN_VERSION@\"

bin_PROGRAMS = blkimage c16image s16image

bindir = $(GIMP_PLUGIN_DIR)/plug-ins

blkimage_SOURCES = \
	plugin-intl.h	\
	blkimage.c 

c16image_SOURCES = \
	plugin-intl.h	\
	c16image.c 

s16image_SOURCES = \
	plugin-intl.h	\
	s16image.c 


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
