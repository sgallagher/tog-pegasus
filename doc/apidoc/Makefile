ROOT = $(PEGASUS_ROOT)
PROGRAM_NAME=apidoc

DIR = $(PROGRAM_NAME)
include $(ROOT)/mak/config.mak

DOC_ROOT = $(ROOT)/doc
DOC_DIR = $(DOC_ROOT)/$(DIR)
DOC_OUTPUT = $(HOME_DIR)/$(DIR)html
DOC_HEAD = $(PROGRAM_NAME).dxx

all:
	$(RM) $(DOC_OUTPUT)/*
	docxx --dir $(DOC_OUTPUT)  --verbose -H $(DOC_HEAD)
	$(COPY) $(DOC_DIR)/*.gif $(DOC_OUTPUT)
	$(COPY) $(DOC_DIR)/*.htm $(DOC_OUTPUT)
	$(COPY) $(DOC_DIR)/*.html $(DOC_OUTPUT)
	$(COPY) $(DOC_DIR)/*.shtml $(DOC_OUTPUT)	
clean:
	$(RM) $(DOC_OUTPUT)/*
depend:

tests: