ROOT = ../../../..

DIR = Pegasus/Handler/snmpIndicationHandler

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_HANDLER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcommon \
    pegconfig \
    peggetoopt \
    pegclient

SOURCES = snmpIndicationHandler.cpp

ifdef PEGASUS_USE_EMANATE
 EXTRA_LIBRARIES += -L$(PEGASUS_EMANATE_LIB_DIR) -lsubagent -lpack -lagent -lemanate -lmibtable -lsnmp

 EXTRA_INCLUDES = -I$(PEGASUS_EMANATE_INCLUDE_DIR) \
        -I$(PEGASUS_EMANATE_INCLUDE_DIR)/snmpd \
        $(SYS_INCLUDES) $(ACE_INCLUDES)

LOCAL_DEFINES += -DSETS -DSR_SNMPv1 -DSR_SNMPv2c -DSR_SNMPv2  -DEMANATE -DSR_ANSI_PROTOTYPES -DSUBAGENT -DSR_USER_AP_SERVICE -DSR_UDS_IPC -DSR_MIB_TABLE_HASHING -DSR_IP

 SOURCES += snmpDeliverTrap_emanate.cpp
else
 SOURCES += snmpDeliverTrap_stub.cpp
endif

LIBRARY = snmpIndicationHandler
 
include $(ROOT)/mak/library.mak

tests:
