ROOT = ../../../../..
DIR = Clients/CGIClient/tests/CGIQueryString
include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

SYS_INCLUDES += -I../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

LIBRARIES = \
    pegcommon

PROGRAM = TestCGIQueryString

SOURCES = \
    ../../CGIQueryString.cpp \
    CGIQueryString.cpp

ifeq ($(OS),VMS)
TMPVMSOBJECTS = $(SOURCES:.cpp=$(OBJ),pegasus_vmshome:[$(OBJ_VMSDIRB).Clients.CGIClient]CGIQueryString.obj,)

SOURCES = \
        CGIQueryString.cpp

endif

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

