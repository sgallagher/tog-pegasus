ROOT = ../../../../..
DIR = Pegasus/Common/tests/ThreadPool
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = ThreadPool
SOURCES = ThreadPool.cpp

include $(ROOT)/mak/program.mak

##
## This test fails intermitently on many platorms if it is not run in 
## verbose mode (see bugzillia #2239).
## It is know to run on Windows without verbosity
## 
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC) 
tests:
	$(PROGRAM)
else
tests:
	@ $(MAKE) tests2 PEGASUS_TEST_VERBOSE=1
endif

tests2:
	$(PROGRAM)

poststarttests:
