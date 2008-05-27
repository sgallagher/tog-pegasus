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
# Makes the dynamic load libs for the files named in DIR
ROOT = ../../../..

DIR = Pegasus/ControlProviders/CertificateProvider

include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
    ifdef OPENSSL_HOME
        SYS_INCLUDES += -I$(OPENSSL_HOME)/include
    endif
else
    $(error PEGASUS_HAS_SSL not defined)
endif

LOCAL_DEFINES = -DPEGASUS_CERTIFICATEPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

ifeq ($(OS_TYPE),vms)
    STATIC=1
endif

LIBRARY = CertificateProvider

LIBRARIES = \
    pegrepository \
    pegprovider \
    pegconfig \
    pegcommon

ifeq ($(PEGASUS_PLATFORM),PASE_ISERIES_IBMCXX)
    LIBRARIES += ILEWrapperUtils
endif

SOURCES = CertificateProvider.cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
    NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegconfig -lpegprovider
endif

ifdef PEGASUS_HAS_SSL
    ifeq ($(OS_TYPE),windows)
        SYS_LIBS += /libpath:$(OPENSSL_HOME)/lib libeay32.lib ssleay32.lib
    else
        ifdef OPENSSL_HOME
            EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib
        endif
        EXTRA_LIBRARIES += -lssl -lcrypto
    endif
endif

ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
    AIX_LIB_PRIORITY=-20
endif

ifeq ($(PEGASUS_PLATFORM),PASE_ISERIES_IBMCXX)
    AIX_LIB_PRIORITY=-20
endif

ifeq ($(OS_TYPE),windows)
    SYS_LIBS += ws2_32.lib advapi32.lib netapi32.lib
endif

include $(ROOT)/mak/dynamic-library.mak
