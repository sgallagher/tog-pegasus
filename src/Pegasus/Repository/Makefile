ROOT = ../../..

DIR = Pegasus/Repository

include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL

LIBRARIES = pegcommon pegconfig

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
