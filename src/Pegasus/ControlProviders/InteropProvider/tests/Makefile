ROOT=$(PEGASUS_ROOT)
DIR = Pegasus/ControlProviders/InteropProvider/tests
include $(ROOT)/mak/config.mak
# include ../../libraries.mak

LIBRARIES= \
    pegcompiler \
    pegrepository \
    pegclient \
    pegcommon
    
LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = testinterop
SOURCES = interop.cpp

include $(ROOT)/mak/program.mak

tests:

 
poststarttests:
	$(PROGRAM)
