ROOT = ../../../..

DIR = Pegasus/Security/Authentication

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_SECURITY_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegauthentication

LIBRARIES = \
    pegcommon \
    peguser   \
    pegconfig

PRE_DEPEND_INCLUDES = -I./depends

ifeq ($(OS),HPUX)
    EXTRA_LIBRARIES += -lsec 
endif

SOURCES = \
    LocalAuthFile.cpp \
    SecureLocalAuthenticator.cpp \
    LocalAuthenticationHandler.cpp \
    SecureBasicAuthenticator.cpp \
    PAMBasicAuthenticator.cpp \
    BasicAuthenticationHandler.cpp \
    AuthenticationManager.cpp

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
