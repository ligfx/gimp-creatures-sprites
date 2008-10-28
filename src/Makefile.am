## Process this file with automake to produce Makefile.in

bin_PROGRAMS = blkimage c16image s16image

bindir = $(GIMP_LIBDIR)/plug-ins

blkimage_SOURCES = \
	plugin-intl.h	\
	blkimage.c 

c16image_SOURCES = \
	plugin-intl.h	\
	c16image.c 

s16image_SOURCES = \
	plugin-intl.h	\
	s16image.c 

AM_CPPFLAGS = \
	-DLOCALEDIR=\""$(LOCALEDIR)"\"		\
	-DDATADIR=\""$(DATADIR)"\" 

INCLUDES =\
	-I$(top_srcdir)		\
	@GIMP_CFLAGS@		\
	-I$(includedir)		\
	@C16_CFLAGS@

LDADD = $(GIMP_LIBS) $(C16_LIBS)
