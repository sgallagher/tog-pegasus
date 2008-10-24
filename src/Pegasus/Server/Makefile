#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
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
ROOT = ../../..

DIR = Pegasus/Server

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

STATIC=1
LIBRARY = pegserver

LIBRARIES = \
	pegcommon \
	pegrepository \
	pegprovider \
	pegexportserver \
	pegpmservice \
	pegprovidermanager \
	pegconfig \
	peguser \
	pegauthentication \
	peghandlerservice \
	pegindicationservice \
	pegquerycommon \
	pegwql \
	pegclient \
	pegprm \
	DefaultProviderManager \
	ConfigSettingProvider \
	UserAuthProvider \
	ProviderRegistrationProvider\
	NamespaceProvider

ifeq ($(PEGASUS_ENABLE_INTEROP_PROVIDER),true)
LIBRARIES += \
	InteropProvider
ifdef PEGASUS_SLP_REG_TIMEOUT
LIBRARIES += \
	pegslp_client
endif
endif

ifndef PEGASUS_DISABLE_PERFINST
LIBRARIES += \
	CIMOMStatDataProvider \
	InteropProvider
endif

ifeq ($(PEGASUS_ENABLE_CQL),true)
LIBRARIES += \
	CIMQueryCapabilitiesProvider
endif

ifdef PEGASUS_HAS_SSL
LIBRARIES += \
	CertificateProvider
endif

ifeq ($(PEGASUS_ENABLE_PROTOCOL_WSMAN),true)
    LIBRARIES += pegwsmserver
endif

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
	CIMOperationRequestDecoder.cpp \
	CIMOperationResponseEncoder.cpp \
	CIMOperationRequestDispatcher.cpp \
	CIMOperationRequestAuthorizer.cpp \
	HTTPAuthenticatorDelegator.cpp \
	ShutdownProvider.cpp \
	ShutdownService.cpp \
	CIMServer.cpp \
	CIMServerState.cpp \
	reg_table.cpp \
	QuerySupportRouter.cpp \
	WQLOperationRequestDispatcher.cpp

ifeq ($(OS),zos)
SOURCES += \
	ConsoleManager_zOS.cpp
endif

ifeq ($(OS_TYPE),windows)
SYS_LIBS = ws2_32.lib advapi32.lib
endif

include $(ROOT)/mak/library.mak
