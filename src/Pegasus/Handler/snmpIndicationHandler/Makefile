ROOT = ../../../..

DIR = Pegasus/Handler/snmpIndicationHandler

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_HANDLER_INTERNAL

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)peggetoopt$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX) 

SOURCES = snmpIndicationHandler.cpp

ifdef PEGASUS_AGENT
 SOURCES += snmpDeliverTrap_emanate.cpp
else
 SOURCES += snmpDeliverTrap_stub.cpp
endif

LIBRARY = snmpIndicationHandler
 
include $(ROOT)/mak/library.mak

tests:
