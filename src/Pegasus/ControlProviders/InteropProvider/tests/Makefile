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

##
## Test is failing on windows and preventing the other test from running.
## see bug 2914.
##
## Test temporarily disabled on windows (bug # 2943 ) 
##
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)  
poststarttests:

else
poststarttests:
	$(PROGRAM)
endif
