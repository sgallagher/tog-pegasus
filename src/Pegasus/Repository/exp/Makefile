ROOT = ../../..

DIR = Pegasus/Repository

include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL

LIBRARIES = pegcommon pegconfig

LIBRARY = pegrepository

SOURCES = \
    InstanceIndexFile.cpp

SOURCESX = \
    AssocClassTable.cpp \
    AssocInstTable.cpp \
    NameSpaceManager.cpp \
    InheritanceTree.cpp \
    InstanceDataFile.cpp \
    InstanceFile.cpp \
    RepositoryDeclContext.cpp \
    CIMRepository.cpp

include $(ROOT)/mak/library.mak
