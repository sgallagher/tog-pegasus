ROOT = ../../..

DIR = Pegasus/Repository

include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL 

LIBRARIES = $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

LIBRARY = pegrepository

SOURCES = \
    NameSpaceManager.cpp \
    InheritanceTree.cpp \
    InstanceIndexFile.cpp \
    CIMRepository.cpp 

include $(ROOT)/mak/library.mak
