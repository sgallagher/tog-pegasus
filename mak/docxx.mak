
.PHONY: docxx

DOC_ROOT = $(ROOT)/doc/reference
DOC_DIR = $(DOC_ROOT)/$(DIR)

docxx:
	$(MKDIRHIER) $(DOC_DIR)
	docxx -d $(DOC_DIR) -H *.h
