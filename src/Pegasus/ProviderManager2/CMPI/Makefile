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

DIR = Pegasus/ProviderManager2/CMPI

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CMPIPM_INTERNAL -DPEGASUS_INTERNALONLY 

LIBRARY = CMPIProviderManager

ifeq ($(OS_TYPE),vms)
 VMS_VECTOR = PegasusCreateProviderManager
endif

LIBRARIES = \
    pegprovider \
    pegprovidermanager \
    pegprm \
    pegclient \
    pegquerycommon \
    pegconfig \
    pegwql

ifndef PEGASUS_DISABLE_CQL
    LIBRARIES += \
       pegcql
endif

LIBRARIES += \
    pegcommon

SOURCES = \
	CMPIProviderManagerMain.cpp \
	CMPIProviderManager.cpp \
	CMPILocalProviderManager.cpp \
	CMPIResolverModule.cpp \
	CMPIProviderModule.cpp \
	CMPIProvider.cpp \
	CMPI_Broker.cpp \
	CMPI_ContextArgs.cpp \
	CMPI_Object.cpp \
	CMPI_BrokerEnc.cpp \
	CMPI_BrokerExt.cpp \
	CMPI_Array.cpp \
	CMPI_DateTime.cpp \
	CMPI_Enumeration.cpp \
	CMPI_Instance.cpp \
	CMPI_ObjectPath.cpp \
	CMPI_Result.cpp \
	CMPI_String.cpp \
	CMPI_ThreadContext.cpp \
	CMPI_Value.cpp \
	CMPI_Query2Dnf.cpp \
	CMPI_Cql2Dnf.cpp \
        CMPI_Wql2Dnf.cpp \
        CMPI_SelectExp.cpp \
        CMPI_SelectCond.cpp \
        CMPI_SubCond.cpp \
        CMPI_Predicate.cpp \
        CMPI_SelectExpAccessor_WQL.cpp \
        CMPI_SelectExpAccessor_CQL.cpp

ifeq ($(OS),VMS)
#
# Large number of objects causes command line to be too long.
# Added them to the linker options file.
#
OBJECTS_IN_OPTIONFILE = YES
endif

include $(ROOT)/mak/dynamic-library.mak
