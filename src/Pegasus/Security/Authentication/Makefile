ROOT = ../../../..

DIR = Pegasus/Security/Authentication

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_SECURITY_INTERNAL

LIBRARY = peglocalauth

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig.x
else
DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegconfig
endif

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = \
    LocalAuthFile.cpp \
    SecureLocalAuthenticator.cpp \
    LocalAuthenticationHandler.cpp \
    BasicAuthenticationHandler.cpp \
    AuthenticationManager.cpp

SYS_LIBS = ws2_32.lib advapi32.lib

include $(ROOT)/mak/library.mak
