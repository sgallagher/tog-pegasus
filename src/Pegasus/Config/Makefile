ROOT = ../../..

DIR = Pegasus/Config

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONFIG_INTERNAL

LIBRARY = pegconfig

LIBRARIES = \
    pegcommon

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
