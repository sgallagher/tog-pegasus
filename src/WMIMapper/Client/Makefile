ROOT = ../../..

DIR = WMIMapper/Client

include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLIENT_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegwmiclient
    
LIBRARIES = \
    pegcommon \
    wmiprovider

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    CIMClient.cpp \
    CIMOperationRequestEncoder.cpp \
    CIMOperationResponseDecoder.cpp \
    ClientAuthenticator.cpp \
    CIMClientException.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

copy2:
	$(COPY) $(ROOT)/src/Pegasus/Client/CIMOperationRequestEncoder.cpp ./CIMOperationRequestEncoder.cpp
	$(COPY) $(ROOT)/src/Pegasus/Client/CIMOperationRequestEncoder.h ./CIMOperationRequestEncoder.h
	$(COPY) $(ROOT)/src/Pegasus/Client/CIMOperationResponseDecoder.cpp ./CIMOperationResponseDecoder.cpp
	$(COPY) $(ROOT)/src/Pegasus/Client/CIMOperationResponseDecoder.h ./CIMOperationResponseDecoder.h
	$(COPY) $(ROOT)/src/Pegasus/Client/ClientAuthenticator.cpp ./ClientAuthenticator.cpp
	$(COPY) $(ROOT)/src/Pegasus/Client/ClientAuthenticator.h ./ClientAuthenticator.h
	$(COPY) $(ROOT)/src/Pegasus/Client/CIMClientException.cpp ./CIMClientException.cpp
	$(COPY) $(ROOT)/src/Pegasus/Client/CIMClientException.h ./CIMClientException.h
	$(COPY) $(ROOT)/src/Pegasus/Client/Linkage.h ./Linkage.h
