ROOT = ../../../..

DIR = Pegasus/Security/Authentication

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_COMMON_INTERNAL

LIBRARY = peglocalauth

SOURCES = \
    LocalAuthFile.cpp \
    SecureLocalAuthenticator.cpp \
    LocalAuthenticationHandler.cpp \
    BasicAuthenticationHandler.cpp \
    AuthenticationManager.cpp

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
