ROOT = ../../..

DIR = WMIMapper/PegWMIServer

include $(ROOT)/mak/config.mak

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
    CIMOperationResponseEncoder.cpp \
    CIMOperationRequestDispatcher.cpp \
    CIMOperationRequestAuthorizer.cpp \
    HTTPAuthenticatorDelegator.cpp \
    CIMServer.cpp \
    CIMServerState.cpp 

OBJECTS = $(SOURCES:.cpp=$(OBJ))

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak

run:
	Server $(REPOSITORY_ROOT)

copy2:
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationRequestDecoder.cpp ./CIMOperationRequestDecoder.cpp
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationResponseEncoder.cpp ./CIMOperationResponseEncoder.cpp
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationRequestAuthorizer.cpp ./CIMOperationRequestAuthorizer.cpp 
	$(COPY) $(ROOT)/src/Pegasus/Server/HTTPAuthenticatorDelegator.cpp ./HTTPAuthenticatorDelegator.cpp
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMServerState.cpp ./CIMServerState.cpp
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationRequestDecoder.h ./CIMOperationRequestDecoder.h
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationResponseEncoder.h ./CIMOperationResponseEncoder.h
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMOperationRequestAuthorizer.h ./CIMOperationRequestAuthorizer.h 
	$(COPY) $(ROOT)/src/Pegasus/Server/HTTPAuthenticatorDelegator.h ./HTTPAuthenticatorDelegator.h
	$(COPY) $(ROOT)/src/Pegasus/Server/CIMServerState.h ./CIMServerState.h
