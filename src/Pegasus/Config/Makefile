ROOT = ../../..

DIR = Pegasus/Config

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONFIG_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegconfig

LIBRARIES = \
	pegcommon

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
EXTRA_LIBRARIES += -lCstd
endif

SOURCES = \
	ConfigExceptions.cpp \
	ConfigFile.cpp \
	ConfigFileHandler.cpp \
	ConfigManager.cpp \
	ConfigPropertyOwner.cpp \
	DefaultPropertyOwner.cpp \
	FileSystemPropertyOwner.cpp \
	LogPropertyOwner.cpp \
	NormalizationPropertyOwner.cpp \
	TracePropertyOwner.cpp \
	SecurityPropertyOwner.cpp \
	RepositoryPropertyOwner.cpp \
	ProviderDirPropertyOwner.cpp \
	ShutdownPropertyOwner.cpp

include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak
