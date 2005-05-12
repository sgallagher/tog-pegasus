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
# Makes the dynamic load libs for the files named in DIR
ROOT = ../../../..

DIR = Pegasus/ControlProviders/CertificateProvider

include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

LOCAL_DEFINES = -DPEGASUS_CERTIFICATEPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = CertificateProvider

LIBRARIES = \
	pegcommon \
	pegconfig \
	pegprovider

SOURCES = CertificateProvider.cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegconfig -lpegprovider
endif

ifeq ($(OS),linux)
  EXTRA_LIBRARIES += -lcrypt
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
    EXTRA_LINK_ARGUMENTS =  -Xlinker -rpath -Xlinker $(OPENSSL_HOME)/lib
  endif
endif

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif


ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
  AIX_LIB_PRIORITY=-20
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(OS),HPUX)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_GNU)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
  ifdef PEGASUS_HAS_SSL
    EXTRA_LIBRARIES += -L$(OPENSSL_HOME)/lib -lssl -lcrypto
  endif
endif

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib netapi32.lib
  ifdef PEGASUS_HAS_SSL
    SYS_LIBS += /libpath:$(OPENSSL_HOME)/lib libeay32.lib ssleay32.lib
  endif
endif

include $(ROOT)/mak/library.mak
