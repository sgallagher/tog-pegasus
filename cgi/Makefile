ROOT = ..

include $(ROOT)/mak/config.mak

all:
	$(COPY) $(BIN_DIR)/CGIClient.exe cgi-bin
	$(COPY) $(BIN_DIR)/*.dll cgi-bin
	$(COPY) $(ACE_ROOT)/bin/*.dll cgi-bin

clean:
	$(RM) cgi-bin/*

depend:

tests:
