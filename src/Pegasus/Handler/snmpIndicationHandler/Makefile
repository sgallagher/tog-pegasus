ROOT = ../../../..

DIR = Pegasus/Handler/snmpIndicationHandler

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_HANDLER_INTERNAL

LIBRARIES = \
    pegcommon \
    pegconfig \
    peggetoopt \
    pegclient

SOURCES = snmpIndicationHandler.cpp

ifdef PEGASUS_AGENT
 SOURCES += snmpDeliverTrap_emanate.cpp
else
 SOURCES += snmpDeliverTrap_stub.cpp
endif

LIBRARY = snmpIndicationHandler
 
include $(ROOT)/mak/library.mak

tests:
