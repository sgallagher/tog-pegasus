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
  ifneq ($(OS),HPUX)
    ifdef PEGASUS_ENABLE_SLP
      LIBRARIES += slp
    endif
  endif
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = cimserver

SOURCES = cimserver.cpp

include $(ROOT)/mak/program.mak

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib	
endif

tests:

poststarttests:

install:
	$(RM) $(PEGASUS_HOME)/cimserver_planned.conf
	$(RM) $(PEGASUS_HOME)/cimserver_current.conf
	$(RM) $(PEGASUS_HOME)/cert.pem
	$(RM) $(PEGASUS_HOME)/file.pem
	$(RM) $(PEGASUS_HOME)/server.pem
	$(RM) $(PEGASUS_HOME)/client.pem
	$(RM) $(PEGASUS_HOME)/cimserver.rnd
	$(RM) $(PEGASUS_HOME)/ssl.rnd
	$(RM) $(PEGASUS_HOME)/ssl.cnf
	$(COPY) cimserver_planned.conf $(PEGASUS_HOME)
	$(COPY) cimserver_current.conf $(PEGASUS_HOME)
	$(COPY) cert.pem $(PEGASUS_HOME)
	$(COPY) file.pem $(PEGASUS_HOME)
	$(COPY) server.pem $(PEGASUS_HOME)
	$(COPY) client.pem $(PEGASUS_HOME)
	$(COPY) cimserver.rnd    $(PEGASUS_HOME)
	$(COPY) ssl.rnd    $(PEGASUS_HOME)
	$(COPY) ssl.cnf    $(PEGASUS_HOME)
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	$(PEGASUS_HOME)/bin/cimserver -install
endif

uninstall:
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	$(PEGASUS_HOME)/bin/cimserver -remove
endif

run: install
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	cimserver -start
else
	cimserver
endif

