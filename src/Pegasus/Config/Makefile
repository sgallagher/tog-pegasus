ROOT = ../../..

DIR = Pegasus/Config

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONFIG_INTERNAL

LIBRARY = pegconfig

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES += \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x
else
 DYNAMIC_LIBRARIES += -lpegcommon
endif

SOURCES = \
	ConfigExceptions.cpp \
	ConfigFile.cpp \
	ConfigFileHandler.cpp \
	ConfigManager.cpp \
	ConfigPropertyOwner.cpp \
	DefaultPropertyOwner.cpp \
	LogPropertyOwner.cpp \
	TracePropertyOwner.cpp \
        SecurityPropertyOwner.cpp \
        RepositoryPropertyOwner.cpp \
        ShutdownPropertyOwner.cpp

include $(ROOT)/mak/library.mak
