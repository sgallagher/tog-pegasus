ROOT = ../..

include $(ROOT)/mak/config.mak

DIR = ProviderSpec
DOC_ROOT = $(ROOT)/doc
DOC_DIR = $(DOC_ROOT)/$(DIR)
# Puts the output into a directory under HOME_DIR called Docnamehtml
DOC_OUTPUT = $(HOME_DIR)/$(DIR)html
DOC_HEAD = ProviderSpec.dxx


all:
	$(RM) $(DOC_OUTPUT)/*
	$(RM) DocTOC.htm
	gawk --file=DocToc.awk $(DOC_HEAD) >DocTOC.htm
	docxx --dir $(DOC_OUTPUT)  --verbose -H $(DOC_HEAD)
	$(COPY) $(DOC_DIR)/*.gif $(DOC_OUTPUT)
	$(COPY) $(DOC_DIR)/*.htm $(DOC_OUTPUT)
#	$(COPY) $(DOC_DIR)/*.css $(DOC_OUTPUT)

clean:
	$(RM) $(DOC_OUTPUT)/*

depend:

tests:  
