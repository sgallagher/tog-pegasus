ROOT = $(PEGASUS_ROOT)
DIRCLIENT = Pegasus/Client
DIR = Pegasus/Client/tests/slp
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifdef PEGASUS_ENABLE_SLP
  FLAGS += -DPEGASUS_ENABLE_SLP
endif

PROGRAM = TestSLPClient

SOURCES = slpclient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
install:
