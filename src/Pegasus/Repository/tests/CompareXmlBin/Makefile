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
#
# CompareXmlBin MAkefile
#
# Commands available are:
#
# make tests			runs the whole test - build, test clean
#
# make repo_build		build the default (Interanl) repository
# make repo_build_whole		builds the repository with all namespaces
# make repo_build_Internal  	build the repository with Interanal namespace 
# make repo_build_Interop	build the repository with InterOp namespace
# make repo_build_ManagedSystem	build the repository with ManagedSystem namespace
#	
# make run_test			runs the test
# make repo_clean		cleans the repsoitory
# make clean			cleans all
#
#
#
ROOT = ../../../../..
DIR = Pegasus/Repository/tests/CompareXmlBin

include $(ROOT)/mak/config.mak
### include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY
LOCAL_CLEAN_TARGET=local-clean

PROGRAM = CompareRepositories
SOURCES = 

##
## REPO_DIR : Directory to build the test repositories in.
##
## NOTE: The shorthand notation of '.' (DOT) for te curent working directory 
##       cannot be used in REPO_DIR. This path specification gets passed to
##       the make repositories makefiles that are executing within the context
##       of the schema directory. This would cause a repository to be created 
##       in each direcory that is being processed as part of te repository
##       build. 
##

## REPO_DIR = $(REPOSITORY_DIR) 
REPO_DIR = $(ROOT)/src/Pegasus/Repository/tests/CompareXmlBin


## REPO_XXX : The file name to be used for each type of test repository.
## 
REPO_XML = repository_xml
REPO_BIN = repository_bin

## include $(ROOT)/mak/program.mak

.PHONY: FORCE repo_build run_test repo_clean tests poststarttests

FORCE: 

#
# This section is to set which platforms the comparerepositories test runs on.
# Following is the complete list of platforms at the time this test was added.
# To enable this test on a platform add the appropriate sequence for it.
#	 ifeq ($(PEGASUS_PLAFORM, XXXXX)
#        PEGASUS_ENABLE_COMPAREREPOSITORIES=true	
#	 endif 
# When enabled on all platforms then this section of the makefile
# can be removed.
#
# AIX_RS_IBMCXX.mak        
# LINUX_ZSERIES_GNU.mak
# DARWIN_PPC_GNU.mak       
# NSK_NONSTOP_NMCPLUS.mak
# HPUX_ACC.mak             	ENABLED Feb 18, 2005 by JRW for Denise
# SOLARIS_SPARC_CC.mak
# HPUX_IA64_ACC.mak  		ENABLED Feb 18, 2005 by JRW for Denise      
# SOLARIS_SPARC_GNU.mak
# HPUX_PARISC_ACC.mak  		ENABLED Feb 18, 2005 by JRW for Denise    
# TRU64_ALPHA_DECCXX.mak
# LINUX_IA64_GNU.mak       	ENABLED Feb 18, 2005 by JRW for Denise
# WIN32_IX86_MSVC.mak		ENABLED Feb 15, 2005 by JRW
# LINUX_IX86_GNU.mak		ENABLED Feb 15, 2005 by JRW
# ZOS_ZSERIES_IBM.mak
# LINUX_PPC_GNU.mak

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC) 
PEGASUS_ENABLE_COMPAREREPOSITORIES=true
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU) 
PEGASUS_ENABLE_COMPAREREPOSITORIES=true 
endif

# the following enables all HPUX platforms 
ifeq ($(OS),HPUX)
PEGASUS_ENABLE_COMPAREREPOSITORIES=true
endif

# Test enabled  for IA64 Linux platforms.
ifeq ($(PEGASUS_PLATFORM),LINUX_IA64_GNU) 
PEGASUS_ENABLE_COMPAREREPOSITORIES=true 
endif

ifdef PEGASUS_ENABLE_COMPAREREPOSITORIES
tests:  repo_build run_test clean
	@ $(ECHO) +++++ CompareXmlBin repository test: FINISHED with test

else

tests:

endif
# endif for ifdef PEGASUS_ENABLE_COMPAREREPOSITORIES


#
# For the following poststarttest rule to work the comparerepositories 
# test will have to understand how to open a connection to the server
# or a file. Build one (XML) repository in the server and the BIN 
# in a local file then compare them. It might be an interesting test
#
# poststarttests: repo_Server_build run_test repo_clean
# 	@ $(ECHO) +++++ CompareXmlBin repository test: FINISHED with test
#

poststarttests:

clean: local-clean

messages:

depend:

repo_clean:
	@ $(RMREPOSITORY) $(REPO_DIR)/$(REPO_XML)
	@ $(RMREPOSITORY) $(REPO_DIR)/$(REPO_BIN)

local-clean: repo_clean
	@ - rm -rf file1 file2

run_test:
	$(PROGRAM) $(REPO_DIR)/$(REPO_XML) $(REPO_DIR)/$(REPO_BIN) 
#
# build both XML and BIN in local files 
#
# The following would build the entire repository with all name spaces
#
#	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_XML) REPOSITORY_MODE=XML
#
# To save time the Internal namespace is built rather than the whole repository
# To build the whole repository or other name namespaces, then use the 
# appropiate repo_build rule dependency
# EG:
#
# repo_build: repo_build_all
# repo_build: repo_build_Internal
# repo_build: repo_build_InterOp
# repo_build: repo_build_ManagedSystems


repo_build: repo_build_Internal

repo_build_all:
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_XML) REPOSITORY_MODE=XML
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_BIN) REPOSITORY_MODE=BIN


repo_build_Internal:
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus/Internal repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_XML) REPOSITORY_MODE=XML
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus/Internal repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_BIN) REPOSITORY_MODE=BIN

repo_build_InterOp:
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus/InterOp repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_XML) REPOSITORY_MODE=XML
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus/InterOp repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_BIN) REPOSITORY_MODE=BIN

repo_build_ManagedSystems:
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus/ManagedSystems repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_XML) REPOSITORY_MODE=XML
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus/ManagedSystems repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_BIN) REPOSITORY_MODE=BIN

#
# build the XML with the server
# build the BIN in a local file
#
repo_Server_build:
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus/Internal repositoryServer REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_XML) REPOSITORY_MODE=XML
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus/Internal repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_BIN) REPOSITORY_MODE=BIN



