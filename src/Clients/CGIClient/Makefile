ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/CGIClient
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = CGIClient
SOURCES = CGIClient.cpp

include $(ROOT)/mak/program.mak

install:
	cp $(PEGASUS_HOME)/bin/CGIClient.exe $(ROOT)/cgi/cgi-bin

tests:

poststarttests:
