ROOT = ../../../../..
DIR = Pegasus/Common/tests/Socket
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = Socket
SOURCES = socket_test.cpp

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib
endif

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

