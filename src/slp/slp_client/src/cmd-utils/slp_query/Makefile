ROOT=$(PEGASUS_ROOT)

DIR = slp/slp_client/src/cmd-utils/slp_query

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegslp_client

PROGRAM = slp_query

SOURCES = slp_query.cpp

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib $(LIB_DIR)/pegslp_client.lib
endif
ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
  SYS_LIBS = -lpegslp_client -L$(LIB_DIR)
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests:

# DO NOT DELETE
