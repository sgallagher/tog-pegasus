ROOT = ..

include $(ROOT)/mak/config.mak

ifeq ($(OS_TYPE),windows)
	LIB_SUFFIX = .dll
	LIB_DIR = $(BIN_DIR)
endif

all:
	$(COPY) $(BIN_DIR)/CGIClient$(EXE) cgi-bin
ifneq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
	$(COPY) $(LIB_DIR)/*$(LIB_SUFFIX) cgi-bin
endif

clean:
	$(RM) cgi-bin/CGIClient.exe
ifneq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
	$(RM) cgi-bin/*$(LIB_SUFFIX)
endif

depend:

tests:
