ROOT = ../../../..

DIR = Pegasus/Security/Authentication

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_COMMON_INTERNAL

LIBRARY = peglocalauth

SOURCES = \
    LocalAuthUtils.cpp \
    Authenticator.cpp \
    LocalAuthenticator.cpp 


SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
