ROOT = ..

include $(ROOT)/mak/config.mak

all:
ifeq ($(OS_TYPE),windows)
	$(COPY) $(BIN_DIR)/CGIClient.exe cgi-bin
	$(COPY) $(BIN_DIR)/*.dll cgi-bin
else
	$(COPY) $(BIN_DIR)/CGIClient cgi-bin
	$(COPY) $(LIB_DIR)/*$(LIB_SUFFIX) cgi-bin
endif

clean:
ifeq ($(OS_TYPE),windows)
	$(RM) cgi-bin/CGIClient.exe
	$(RM) cgi-bin/*.dll
else
	$(RM) cgi-bin/CGIClient
	$(RM) cgi-bin/*$(LIB_SUFFIX)
endif

depend:

tests:
