################################################################################
##
## Get external environment variables. Note that all external environment
## variables begin with "PEGASUS_".
##
################################################################################

ifdef PEGASUS_HOME
  HOME_DIR = $(subst \,/,$(PEGASUS_HOME))
else
  ERROR = pegasus_home_undefined
pegasus_home_undefined:
	@ echo PEGASUS_HOME environment variable undefined
	@ exit 1
endif

VALID_PLATFORMS = \
    WIN32_IX86_MSVC \
    LINUX_IX86_GNU \
    AIX_RS_IBMCXX \
    HPUX_PARISC_ACC \
    TRU64_ALPHA_DECCXX \
    SOLARIS_SPARC_GNU \
    ZOS_ZSERIES_IBM

ifndef PEGASUS_PLATFORM
  ERROR = pegasus_platform_undefined
pegasus_platform_undefined:
	@ echo PEGASUS_PLATFORM environment variable undefined. Please set to\
	    one of the following: $(VALID_PLATFORMS)
	@ exit 1
endif

################################################################################

OBJ_DIR = $(HOME_DIR)/obj/$(DIR)
BIN_DIR = $(HOME_DIR)/bin
LIB_DIR = $(HOME_DIR)/lib
REPOSITORY_ROOT = $(HOME_DIR)/repository

LEX = flex

################################################################################
##
## Attempt to include a platform configuration file:
##
################################################################################

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),TRU64_ALPHA_DECCXX)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_GNU)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
   include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
   FOUND = true
endif

ifneq ($(FOUND),true)
  ERROR = pegasus_unknown_platform
pegasus_unknown_platform:
	@ echo PEGASUS_PLATFORM environment variable must be set to one of\
	    the following: $(VALID_PLATFORMS)
	@ exit 1
endif
