ROOT = ../../../..

DIR = Pegasus/Handler/snmpIndicationHandler

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_HANDLER_INTERNAL

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)peggetoopt$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegcliutils$(LIB_SUFFIX) \

SOURCES = snmpIndicationHandler.cpp snmpDeliverTrap_emanate.cpp snmpDeliverTrap_stub.cpp

LIBRARY = snmpIndicationHandler
 
include $(ROOT)/mak/library.mak

tests:
