ROOT = ..

include $(ROOT)/mak/config.mak

ifeq ($(OS_TYPE),windows)
	LIB_SUFFIX = .dll
	LIB_DIR = $(BIN_DIR)
endif

all:
	$(COPY) $(BIN_DIR)/CGIClient$(EXE) cgi-bin
	$(COPY) $(LIB_DIR)/*$(LIB_SUFFIX) cgi-bin

clean:
	$(RM) cgi-bin/CGIClient.exe
	$(RM) cgi-bin/*$(LIB_SUFFIX)

depend:

tests:
