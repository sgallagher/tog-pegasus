ROOT = ../../../..

DIR = Pegasus/Security/Authentication

include $(ROOT)/mak/config.mak

ifdef PEGASUS_WMIMAPPER
  FLAGS += -DPEGASUS_WMIMAPPER
endif

# Use Separate PAM Authentication
ifdef PEGASUS_USE_PAM_STANDALONE_PROC
  FLAGS += -DPEGASUS_USE_PAM_STANDALONE_PROC
endif

LOCAL_DEFINES = -DPEGASUS_SECURITY_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegauthentication

LIBRARIES = \
    pegcommon \
    peguser   \
    pegconfig

ifdef PEGASUS_KERBEROS_AUTHENTICATION
  LIBRARIES += pegkrb
endif


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

ifdef PEGASUS_KERBEROS_AUTHENTICATION
  SOURCES += CIMKerberosAuthenticator.cpp \
             KerberosAuthenticationHandler.cpp
endif


SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
