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

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:

