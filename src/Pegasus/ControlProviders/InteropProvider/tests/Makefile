ROOT=$(PEGASUS_ROOT)
DIR = Pegasus/ControlProviders/InteropProvider/tests
include $(ROOT)/mak/config.mak

LIBRARIES = \
    InteropProvider \
    pegcompiler \
    pegrepository \
    pegclient \
    pegcommon
    
LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestInterop
SOURCES = interop.cpp

include $(ROOT)/mak/program.mak

tests:
install:
 
poststarttests:
	$(PROGRAM)
