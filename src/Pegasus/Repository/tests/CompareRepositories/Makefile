ROOT = ../../../../..
DIR = Pegasus/CompareRepositories/tests/CompareRepositories
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY
LOCAL_CLEAN_TARGET=local-clean

PROGRAM = CompareRepositories
SOURCES = CompareRepositories.cpp

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
REPO_DIR = $(ROOT)/src/Pegasus/Repository/tests/CompareRepositories


## REPO_XXX : The file name to be used for each type of test repository.
## 
REPO_XML = repository_xml
REPO_BIN = repository_bin

include $(ROOT)/mak/program.mak

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
# HPUX_ACC.mak             
# SOLARIS_SPARC_CC.mak
# HPUX_IA64_ACC.mak        
# SOLARIS_SPARC_GNU.mak
# HPUX_PARISC_ACC.mak      
# TRU64_ALPHA_DECCXX.mak
# LINUX_IA64_GNU.mak       
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


ifdef PEGASUS_ENABLE_COMPAREREPOSITORIES
tests:  repo_build run_test repo_clean
	@ $(ECHO) +++++ Compare reposititory test: FINISHED with test



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
# 	@ $(ECHO) +++++ Compare reposititory test: FINISHED with test
#

poststarttests:

install:


repo_clean: 
	  $(RMREPOSITORY) $(REPO_DIR)/$(REPO_XML)
	  $(RMREPOSITORY) $(REPO_DIR)/$(REPO_BIN)

local-clean: repo_clean
	@ - rm -rf file1 file2

run_test:
	@ $(ECHO) +++++ Compare Repositories test: RUNNING TESTS
	$(PROGRAM) $(REPO_DIR)/$(REPO_XML) $(REPO_DIR)/$(REPO_BIN) 
#
# build both XML and BIN in local files
#
repo_build:
	@ $(ECHO) 
	@ $(ECHO) +++++ Compare Repositories test: NOW BUILDING XML reposititory
	@ $(ECHO) +++++     $(REPO_DIR)/$(REPO_XML)
	@ $(ECHO)
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_XML) REPOSITORY_MODE=XML
	@ $(ECHO)  
	@ $(ECHO) +++++ Compare Repositories test: NOW BUILDING BIN reposititories for compare test
	@ $(ECHO) +++++     $(REPO_DIR)/$(REPO_BIN)
	@ $(ECHO) 
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_BIN) REPOSITORY_MODE=BIN

#
# build the XML with the server
# build the BIN in a local file
#
repo_Server_build:
	@ $(ECHO) 
	@ $(ECHO) +++++ Compare Repositories test: NOW BUILDING XML reposititory
	@ $(ECHO) +++++     $(REPO_DIR)/$(REPO_XML)
	@ $(ECHO)
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus repositoryServer REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_XML) REPOSITORY_MODE=XML
	@ $(ECHO)  
	@ $(ECHO) +++++ Compare Repositories test: NOW BUILDING BIN reposititories for compare test
	@ $(ECHO) +++++     $(REPO_DIR)/$(REPO_BIN)
	@ $(ECHO) 
	@ $(MAKE) "-SC" $(ROOT)/Schemas/Pegasus repository REPOSITORY_DIR=$(REPO_DIR) REPOSITORY_NAME=$(REPO_BIN) REPOSITORY_MODE=BIN



