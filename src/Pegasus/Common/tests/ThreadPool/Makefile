ROOT = ../../../../..
DIR = Pegasus/Common/tests/ThreadPool
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = ThreadPool
SOURCES = ThreadPool.cpp

include $(ROOT)/mak/program.mak

##
## This test fails intermitently on HPUX if it is not run in verbose mode
## see bugzillia #2239
## 
ifeq ($(OS),HPUX)
tests: 
	@ $(MAKE) tests2 PEGASUS_TEST_VERBOSE=1
else
tests:
	$(PROGRAM)
endif


tests2:
	$(PROGRAM)

poststarttests:
install:
