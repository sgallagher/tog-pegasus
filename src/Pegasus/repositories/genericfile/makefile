ROOT = ../../../..

DIR = Pegasus/Repositories/genericfile

include $(ROOT)/mak/config.mak

INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL

LIBRARIES = $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
            $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
 DYNAMIC_LIBRARIES = $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
                     $(LIB_DIR)/$(LIB_PREFIX)pegconfig.x
else
 DYNAMIC_LIBRARIES = -lpegcommon \
                     -lpegconfig
endif

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
