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

ifdef PEGASUS_AGENT
 LIBRARIES += 	subagent \
		pack \
		agent \
		emanate \
		mibtable \
		snmp

 EXTRA_INCLUDES = -g  -I/opt/snmp15.3/sadk/include \
        -I/opt/snmp15.3/sadk/include/snmpd \
        -DSETS -DSR_SNMPv1 -DSR_SNMPv2c -DSR_SNMPv2  -DEMANATE -DSR_ANSI_PROTOTYPES \
        -DSUBAGENT -DSR_USER_AP_SERVICE -DSR_UDS_IPC -DSR_MIB_TABLE_HASHING -DSR_IP \
        $(SYS_INCLUDES) $(ACE_INCLUDES)

 SOURCES += snmpDeliverTrap_emanate.cpp
else
 SOURCES += snmpDeliverTrap_stub.cpp
endif

LIBRARY = snmpIndicationHandler
 
include $(ROOT)/mak/library.mak

tests:
