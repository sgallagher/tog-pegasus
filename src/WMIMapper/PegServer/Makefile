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
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationResponseEncoder.cpp ./CIMOperationResponseEncoder.cpp
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationResponseEncoder.h ./CIMOperationResponseEncoder.h
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationRequestAuthorizer.cpp ./CIMOperationRequestAuthorizer.cpp 
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationRequestAuthorizer.h ./CIMOperationRequestAuthorizer.h 
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMServerState.cpp ./CIMServerState.cpp
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMServerState.h ./CIMServerState.h
	$(COPY) $(ROOT)/src/Pegasus/Server/HTTPAuthenticatorDelegator.h ./HTTPAuthenticatorDelegator.h

clean2:
	$(RM) ./CIMOperationResponseEncoder.cpp
	$(RM) ./CIMOperationResponseEncoder.h
	$(RM) ./CIMOperationRequestAuthorizer.cpp 
	$(RM) ./CIMOperationRequestAuthorizer.h 
	$(RM) ./CIMServerState.cpp
	$(RM) ./CIMServerState.h
	$(RM) ./HTTPAuthenticatorDelegator.h
