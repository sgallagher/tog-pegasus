ROOT = ../../../../..
DIR = Pegasus/Client/tests/DeleteNamespace
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
ifneq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
LIBRARIES += slp
endif
endif

PROGRAM = DeleteNamespace

SOURCES = DeleteNamespace.cpp

include $(ROOT)/mak/program.mak

tests:

clean:

poststarttests: 
	$(PROGRAM)
