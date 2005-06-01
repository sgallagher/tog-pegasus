#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
ROOT = ../../..

DIR = Pegasus/Repository

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = -I..

LOCAL_DEFINES = -DPEGASUS_REPOSITORY_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARIES = pegcommon pegquerycommon pegconfig

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

########################################################################
########################################################################
##
## 
## The PEGASUS_ENABLE_REPOSITORY_INSTANCE_FILTER build config variable turns on
## the filtering of enumerate instances for localOnly, includeQualifiers,
## IncludClassOrigin, deepInheritance, and PropertyList. 
##
##  (See bug reports 1046, 2334, 1975, 1046, 3270)
##
##
########################################################################
########################################################################


ifdef PEGASUS_ENABLE_REPOSITORY_INSTANCE_FILTER
  LOCAL_DEFINES += -DPEGASUS_ENABLE_REPOSITORY_INSTANCE_FILTER
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
