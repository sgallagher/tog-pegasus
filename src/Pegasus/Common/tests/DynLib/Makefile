ROOT = ../../../../..
include ../libraries.mak

DIR = Pegasus/Common/tests/DynLib

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

ifeq ($(OS_TYPE),vms)
 VMS_VECTOR = callme
endif

SOURCES = DynLib.cpp

LIBRARY = TestDynLib

include $(ROOT)/mak/dynamic-library.mak

tests:

poststarttests:
