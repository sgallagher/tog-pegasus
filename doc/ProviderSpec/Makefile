ROOT = ../..

include $(ROOT)/mak/config.mak

DIR = ProviderSpec
DOC_ROOT = $(ROOT)/doc/
DOC_DIR = $(DOC_ROOT)/$(DIR)
DOC_OUTPUT = $(HOME_DIR)/$(DIR)/html
DOC_HEAD = ProviderSpec.dxx


all:
	$(RM) $(DOC_OUTPUT)/*	
	gawk --file=PegasusDocTOC.awk $(DOC_HEAD) >PegasusDocTOC.htm
	docxx --dir $(DOC_OUTPUT)  --verbose -H $(DOCHEAD)
	mu copy *.gif $(DOC_OUTPUT)
	mu copy *.htm $(DOC_OUTPUT)
	mu copy *.css $(DOC_OUTPUT)

clean:
	$(RM) $(DOC_OUTPUT)/*

depend:

tests:  