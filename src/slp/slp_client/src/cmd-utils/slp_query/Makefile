ROOT=$(PEGASUS_ROOT)

DIR = slp/slp_client/src/cmd-utils/slp_query

include $(ROOT)/mak/config.mak

PROGRAM = slp_query

SOURCES = slp_query.cpp

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib $(LIB_DIR)/slp_client.lib
endif

include $(ROOT)/mak/program.mak

# DO NOT DELETE
