ROOT = ../../..

DIR = Pegasus/CGI

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX) \
    $(ACE_LIB)

include $(ROOT)/mak/config.mak

PROGRAM = CGIClient
SOURCES = CGIClient.cpp

include $(ROOT)/mak/program.mak

install:
	cp $(ROOT)/bin/CGIClient $(ROOT)/cgi-bin/CGIClient

tests:
