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
## PEP214 Compressed repository conditional compile
##    to enable define PEGASUS_COMPRESS_REPOSITORY
##    in the environment.
##
## COMPILATION:
##
## set PEGASUS_COMPRESS_REPOSITORY=1
## export PEGASUS_COMPRESS_REPOSITORY=1
## etc. 
##
## libz is used in the compression logic so it must be installed in
##  a standard location prior to enabling the compressed repository
##  functionality.
##
## WINDOWS platform libz standard install is:
##            "Program Files"\GnuWin32\include
##            "Program Files"\GnuWin32\lib
##  these are the standard locations used by the windows libz package
##  available on www.sourceforge.net specificaly at
## http://gnuwin32.sourceforge.net/packages/zlib.htm
##
## other rlated web pages are:
##
## http://www.winimage.com/zLibDll/
##
## http://www.gzip.org/zlib/
##
## 
## USAGE:
##
## with the compression code enabled and compiled:
##    all repository formats can be read and written as before 
##    including the compressed repository.
##
## To build a compressed repository the repsoitory name must end with the
## string "compressed".
## 
## The default repository name is defined in pegasus/mak/config.mak as 
## REPOSITORY_NAME = repository
## this can be changed to 
## REPOSITORY_NAME = repository_compressed. 
## or any other string ending in compressed to build a compressed repository.
## 
## 

## PEGASUS_COMPRESS_REPOSITORY = 1

ifdef PEGASUS_COMPRESS_REPOSITORY
  LOCAL_DEFINES += -DPEGASUS_COMPRESS_REPOSITORY

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
include $(ROOT)/mak/install.mak

compress:
	make clean
	make depend PEGASUS_COMPRESS_REPOSITORY=1
	make PEGASUS_COMPRESS_REPOSITORY=1