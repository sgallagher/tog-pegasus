ROOT = ../../..

DIR = Pegasus/Repository

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = pegcommon pegquerycommon pegconfig

ifdef PEGASUS_ENABLE_REMOTE_CMPI
LOCAL_DEFINES += -DPEGASUS_ENABLE_REMOTE_CMPI
endif

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
EXTRA_LIBRARIES += -lCstd
endif

##
##  PEP214 Compressed repository
##  ----------------------------
##
##    This PEP added the capability to utilize compression algorithms
##    on the repository. The compressed repository is approximatley 70%
##    smaller than a regular xml text based repository. This allows
##    significant disk space savings and also results in shorter file 
##    load times.
##   
##    The compression logic is installed as a conditionally compiled option. 
##    To enable the compressed repository functionality:
##	1. Install zlib
##	2. define PEGASUS_ENABLE_COMPRESSED_REPOSITORY=on in the environment
##      3. build Pegasus.
##  
##   Please refer to readme.compression in the main pegasus directory 
##   for more information about repository compression.
##

ifdef PEGASUS_ENABLE_COMPRESSED_REPOSITORY
  LOCAL_DEFINES += -DPEGASUS_ENABLE_COMPRESSED_REPOSITORY

  ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
    EXTRA_LINK_FLAGS += -defaultlib:libz -libpath:/"Program Files"/GnuWin32/lib
    EXTRA_INCLUDES += -I/"Program Files"/GnuWin32/include
  else
     EXTRA_LINK_FLAGS += -lz
  endif

endif

ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
## 
## The following .PHONY rule gets around the problem in Windows for 
## the "Program Files" directory name. The depends program strips the
## quotes when the depends file is built and upon subsequent compile
## it fails with no rule to make these fragments.
##
##
.PHONY: /Program Files/GnuWin32/include/zlib.h Files/GnuWin32/include/zconf.h
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
    RepositoryDeclContext.cpp \
    RepositoryQueryContext.cpp

include $(ROOT)/mak/library.mak

compress:
	make clean
	make depend PEGASUS_ENABLE_COMPRESSED_REPOSITORY=1
	make PEGASUS_ENABLE_COMPRESSED_REPOSITORY=1
