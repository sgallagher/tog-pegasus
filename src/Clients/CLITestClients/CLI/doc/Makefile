# Makefile to create documentation from
# nroff formatted for manpages
# This makefile is not connected to the
# standard makefile.  It is strictly a
# means to create the ascii and html
# manpages from an NROFF source.  Note
# that it requires utilities probably only
# found in Linux/Unix environments including.
# nroff and man2html
#
include $(PEGASUS_ROOT)/mak/config.mak

DOCUMENT=CLI
SOURCE=$(DOCUMENT).nroff

html:
	man2html $(SOURCE) >$(DOCUMENT).html


ascii:
	nroff -man $(SOURCE) >$(DOCUMENT).txt 
