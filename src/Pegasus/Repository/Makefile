ROOT = ../../..

DIR = Pegasus/Repository

include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = pegcommon pegconfig

ifdef PEGASUS_ENABLE_REMOTE_CMPI
LOCAL_DEFINES += -DPEGASUS_ENABLE_REMOTE_CMPI
endif

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
EXTRA_LIBRARIES += -lCstd
endif

LIBRARY = pegrepository

SOURCES = \
    InstanceIndexFile.cpp \
    InstanceDataFile.cpp \
    CIMRepository.cpp \
    AssocClassTable.cpp \
    AssocInstTable.cpp \
    NameSpaceManager.cpp \
    InheritanceTree.cpp \
    RepositoryDeclContext.cpp

include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak
