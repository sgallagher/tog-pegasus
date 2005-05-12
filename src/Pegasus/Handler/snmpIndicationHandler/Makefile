#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
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
