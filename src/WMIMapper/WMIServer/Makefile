ROOT = ../../..

DIR = WMIMapper/WMIServer

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegconfig \
    pegwmiserver \
    pegclient

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifneq ($(OS),HPUX)
    ifndef NO_SLP
      LIBRARIES += slp
    endif
  endif
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = WMIServer

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
	$(RM) $(PEGASUS_HOME)/server.pem
	$(RM) $(PEGASUS_HOME)/client.pem
	$(RM) $(PEGASUS_HOME)/ssl.rnd
	$(RM) $(PEGASUS_HOME)/ssl.cnf
	$(COPY) cimserver_planned.conf $(PEGASUS_HOME)
	$(COPY) cimserver_current.conf $(PEGASUS_HOME)
	$(COPY) $(PEGASUS_ROOT)/server.pem $(PEGASUS_HOME)
	$(COPY) $(PEGASUS_ROOT)/client.pem $(PEGASUS_HOME)
	$(COPY) $(PEGASUS_ROOT)/ssl.rnd    $(PEGASUS_HOME)
	$(COPY) $(PEGASUS_ROOT)/ssl.cnf    $(PEGASUS_HOME)

run: install
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	cimserver -l
else
	cimserver
endif

copy2:
	$(COPY) $(ROOT)/src/Server/service.cpp ./service.cpp
	$(COPY) $(ROOT)/src/Server/service.h ./service.h

clean2:
	$(RM) ./service.cpp
	$(RM) ./service.h

