ROOT = ../../../../..
DIR = Pegasus/Common/tests/Config
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestConfig
SOURCES = Config.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests: 
