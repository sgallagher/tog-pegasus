ROOT = ../../..

DIR = Clients/CGIClient
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = CGIClient
SOURCES = CGIClient.cpp

include $(ROOT)/mak/program.mak

install:
	cp $(PEGASUS_HOME)/bin/CGIClient.exe $(ROOT)/cgi/cgi-bin

tests:

poststarttests:
