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
## PEP214 Compressed repository conditional compile.
##    To enable the compressed repository functionality 
##    define PEGASUS_COMPRESS_REPOSITORY in the environment.
##
##
## COMPILATION:
##
##    To enable the compressed repository functionality 
##    define PEGASUS_COMPRESS_REPOSITORY in the environment,
##    prior to building the tree. Then build the whole source tree.
##
## set PEGASUS_COMPRESS_REPOSITORY=1
## export PEGASUS_COMPRESS_REPOSITORY=1
## etc. 
##
## Alternativly in this directory(pegasus/src/pegasus/Repository type:
##     make compress
##
##
## LIBZ INSTALLATION:
##
## libz is used in the compression logic so it must be installed in
##  a standard location prior to enabling the compressed repository
##  functionality.
##
## LIBZ web pages are:
##
##      http://www.sourceforge.net
##      http://gnuwin32.sourceforge.net/packages/zlib.htm
##      http://www.winimage.com/zLibDll/
##      http://www.gzip.org/zlib/
##
## LIBZ INSTALLATION ON WINDOWS PLATFORMS: 
##      The standard libz install is:
##            "Program Files"\GnuWin32\include
##			zlib.h and zconf.h
##            "Program Files"\GnuWin32\lib
##			libz.a
##            "Program Files"\GnuWin32\bin
##			zlib1.dll
##
## These are the locations used by the windows libz package
## available on www.sourceforge.net specificaly at
## http://gnuwin32.sourceforge.net/packages/zlib.htm.
##
## This make file specifies these directorys for the compilation of the 
## CIMRepository. The CIMserver.exe will use the zlib1.dll and expects to
## find it in one of the pathed directories. 
##  
##
##
## LIBZ INSTALLATION ON UNIX PLATFORMS:
## Install zlib.h & zconf.h in the standard system includes dierctory.
## Install libz.a in the standard systems library directory.
## Install zlib1.dll in the standard executables directory
## 
## USAGE:
##
## With the compression code enabled and compiled:
##    - All repositorys built will be in the compressed format by default.
##    - all repository formats can be read.
##    - all repository formats can be written.
##       To build a non compressed repository with compression enabled:
##       set PEGASUS_COMPRESS_REPSOITORY to "build_non_compresed".
##       This is usedby the CompareXmlCompressed repository test. 
## 
##  export PEGASUS_COMPRESS_REPOSITORY=1
##  export PEGASUS_COMPRESS_REPOSITORY=build_non_compressed
##  set PEGASUS_COMPRESS_REPOSITORY=1
##  set PEGASUS_COMPRESS_REPOSITORY=build_non_compressed
## 
## TESTERS NOTE:
##
## If PEGASUS_COMPRESS_REPOSITORY is defined then the CompareXmlCompressed
## test is run. If it is not defined then the CompareXmlCompresed is not run.
##
## However if you have the zlibs installed and want to noramlly test
## everything with a regular repository but want to run the 
## CompareXmlCompressed test then define PEGASUS_COMPRESS_REPOSITORY_TEST. 
## This will cause the test to run, it will recompile the CIMRepository for 
## compression, run the test and then recompile CIMRepository for
## non-compression.
##
##


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