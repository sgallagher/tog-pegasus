ROOT = ../../..

DIR = Pegasus/Repository

include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL

LIBRARIES = pegcommon pegconfig

LIBRARY = pegrepository

XSOURCES = \
    CIMRepository.cpp \
    AssocClassTable.cpp \
    AssocInstTable.cpp \
    NameSpaceManager.cpp \
    InheritanceTree.cpp \
    RepositoryDeclContext.cpp

SOURCES = \
    CIMRepository.cpp \
    InstanceIndexFile.cpp \
    InstanceDataFile.cpp \
    $(XXSOURCES)

include $(ROOT)/mak/library.mak
