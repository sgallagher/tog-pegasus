ROOT = ../..

DIR = DevManual
include $(ROOT)/mak/config.mak

DOC_ROOT = $(ROOT)/doc
DOC_DIR = $(DOC_ROOT)/$(DIR)
DOC_OUTPUT = $(HOME_DIR)/$(DIR)html
DOC_HEAD = PegasusDoc.dxx


all:
	$(RM) $(DOC_OUTPUT)/*
	gawk --file=PegasusDocTOC.awk $(DOC_HEAD) >pegdocsidebarleft.shtml
	docxx --dir $(DOC_OUTPUT)  --verbose -H $(DOC_HEAD)
	$(COPY) $(DOC_DIR)/*.gif $(DOC_OUTPUT)
	$(COPY) $(DOC_DIR)/*.htm $(DOC_OUTPUT)
	$(COPY) $(DOC_DIR)/*.html $(DOC_OUTPUT)
	$(COPY) $(DOC_DIR)/*.shtml $(DOC_OUTPUT)	
	$(RM) pegdocsidebarleft.shtml
clean:
	$(RM) $(DOC_OUTPUT)/*

depend:

tests: