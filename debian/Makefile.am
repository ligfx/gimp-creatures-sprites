## $Id: Makefile.am,v 1.1 2001/09/04 18:51:53 cevina Exp $
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
