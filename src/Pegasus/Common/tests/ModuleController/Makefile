ROOT = ../../../../..
DIR = Pegasus/Common/tests/ModuleController
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = mod_control	
SOURCES = mod_control.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
