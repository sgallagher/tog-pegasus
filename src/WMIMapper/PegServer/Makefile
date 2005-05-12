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
ROOT = ../../..

DIR = WMIMapper/PegServer

include $(ROOT)/mak/config.mak

# SSL support
ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_SERVER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegwmiserver

LIBRARIES = \
    pegcommon \
    pegrepository \
    pegexportserver \
    pegconfig \
    peguser \
    pegauthentication \
    pegwql \
    wmiprovider

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMOperationRequestDecoder.cpp \
    HTTPAuthenticatorDelegator.cpp \
    CIMServer.cpp \
    CIMOperationResponseEncoder.cpp \
    CIMOperationRequestDispatcher.cpp \
    CIMOperationRequestAuthorizer.cpp \
    CIMServerState.cpp \
	WMIMapperUserInfoContainer.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib
ifdef PEGASUS_HAS_SSL
  SYS_LIBS += $(OPENSSL_HOME)/Lib/*.lib
endif

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)

copy2:
	$(COPY) "$(ROOT)/src/Pegasus/Server/CIMOperationResponseEncoder.cpp" "./CIMOperationResponseEncoder.cpp"
	$(COPY) "$(ROOT)/src/Pegasus/Server/CIMOperationResponseEncoder.h" "./CIMOperationResponseEncoder.h"
	$(COPY) "$(ROOT)/src/Pegasus/Server/CIMOperationRequestAuthorizer.cpp" "./CIMOperationRequestAuthorizer.cpp"
	$(COPY) "$(ROOT)/src/Pegasus/Server/CIMOperationRequestAuthorizer.h" "./CIMOperationRequestAuthorizer.h"
	$(COPY) "$(ROOT)/src/Pegasus/Server/CIMServerState.cpp" "./CIMServerState.cpp"
	$(COPY) "$(ROOT)/src/Pegasus/Server/CIMServerState.h" "./CIMServerState.h"
	$(COPY) "$(ROOT)/src/Pegasus/Server/HTTPAuthenticatorDelegator.h" "./HTTPAuthenticatorDelegator.h"

clean2:
	$(RM) ./CIMOperationResponseEncoder.cpp
	$(RM) ./CIMOperationResponseEncoder.h
	$(RM) ./CIMOperationRequestAuthorizer.cpp 
	$(RM) ./CIMOperationRequestAuthorizer.h 
	$(RM) ./CIMServerState.cpp
	$(RM) ./CIMServerState.h
	$(RM) ./HTTPAuthenticatorDelegator.h
