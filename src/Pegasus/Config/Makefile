ROOT = ../../..

DIR = Pegasus/Config

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONFIG_INTERNAL

LIBRARY = pegconfig

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

DYNAMIC_LIBRARIES += -lpegcommon 

SOURCES = \
	ConfigExceptions.cpp \
	ConfigFile.cpp \
	ConfigFileHandler.cpp \
	ConfigManager.cpp \
	DefaultPropertyOwner.cpp \
	LogPropertyOwner.cpp \
	TracePropertyOwner.cpp

include $(ROOT)/mak/library.mak
