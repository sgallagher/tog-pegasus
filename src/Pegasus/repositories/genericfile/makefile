ROOT = ../../../..

DIR = Pegasus/Repositories/genericfile

include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL

LIBRARIES = pegcommon \
            pegconfig

LIBRARY = pegrepository

SOURCES = \
    AssocClassTable.cpp \
    AssocInstTable.cpp \
    NameSpaceManager.cpp \
    InheritanceTree.cpp \
    InstanceIndexFile.cpp \
    InstanceFile.cpp \
    RepositoryDeclContext.cpp \
    CIMRepository.cpp

include $(ROOT)/mak/library.mak
