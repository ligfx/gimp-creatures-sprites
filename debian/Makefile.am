## $Id: Makefile.am,v 1.2 2001/09/18 18:40:15 cevina Exp $
## Process this file with automake to produce Makefile.in

EXTRA_DIST = \
	changelog \
	control \
	copyright \
	docs \
	creatures-sprites.doc-base \
	rules

.PHONY: files

files:
	@files=`ls $(DISTFILES) 2> /dev/null`; for p in $$files; do \
	  echo $$p; \
	done
