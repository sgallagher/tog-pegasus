ROOT = ../..
DIR = Server
include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegconfig \
    pegrepository \
    pegserver \
    pegprovider \
    pegwql \
    pegclient 

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    LIBRARIES += slp		
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimserver

SOURCES = cimserver.cpp

include $(ROOT)/mak/program.mak

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib	
endif

tests:

poststarttests:

install:
	$(COPY) cimserver_planned.conf $(PEGASUS_HOME)
	$(COPY) cimserver_current.conf $(PEGASUS_HOME)
	$(COPY) $(PEGASUS_ROOT)/server.pem $(PEGASUS_HOME)
	$(COPY) $(PEGASUS_ROOT)/ssl.rnd    $(PEGASUS_HOME)

run: install
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	cimserver -l
else
	cimserver
endif

