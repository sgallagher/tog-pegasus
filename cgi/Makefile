ROOT = ..

include $(ROOT)/mak/config.mak

all:
ifeq ($(OS_TYPE),windows)
	$(COPY) $(BIN_DIR)/CGIClient.exe cgi-bin
	$(COPY) $(BIN_DIR)/*.dll cgi-bin
	$(COPY) $(ACE_ROOT)/bin/*.dll cgi-bin
else
	$(COPY) $(BIN_DIR)/CGIClient cgi-bin
	$(COPY) $(LIB_DIR)/*.so cgi-bin
	$(COPY) $(ACE_ROOT)/ace/libACE.so cgi-bin
endif

clean:
	$(RM) cgi-bin/*

depend:

tests:
