ROOT = ../../..

DIR = Pegasus/Clients/wbemexec

include $(ROOT)/mak/config.mak

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)peggetoopt$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegcliutils$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol.x \
    $(LIB_DIR)/$(LIB_PREFIX)peggetoopt.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegcliutils.x
else
DYNAMIC_LIBRARIES += -lpegcommon \
-lpegprotocol \
-lpeggetoopt \
-lpegrepository \
-lpegclient \
-lpegcliutils
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = wbemexec

SOURCES = WbemExecException.cpp \
          WbemExecClientHandler.cpp \
          WbemExecClientHandlerFactory.cpp \
          XMLProcess.cpp \
          WbemExecCommand.cpp

include $(ROOT)/mak/program.mak

tests:

install:
