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

ifdef PEGASUS_ROOT
    ROOT =  $(subst \,/,$(PEGASUS_ROOT))
else
    ERROR = pegasus_root_undefined
pegasus_root_undefined:
	@ echo PEGASUS_ROOT environment variable undefined
	@ exit 1
endif

# l10n
ifdef ICU_ROOT
    ICUROOT =  $(subst \,/,$(ICU_ROOT))
endif

ifdef PEGASUS_TMP
  TMP_DIR = $(subst \,/,$(PEGASUS_TMP))
else
  TMP_DIR = .
endif

ifdef PEGASUS_DISPLAYCONSUMER_DIR
  DISPLAYCONSUMER_DIR = $(subst \,/,$(PEGASUS_DISPLAYCONSUMER_DIR))
else
  DISPLAYCONSUMER_DIR = $(PEGASUS_HOME)
endif

VALID_PLATFORMS = \
    WIN32_IX86_MSVC \
    LINUX_IX86_GNU \
    LINUX_PPC_GNU \
    LINUX_IA64_GNU \
    LINUX_ZSERIES_GNU \
    AIX_RS_IBMCXX \
    HPUX_PARISC_ACC \
    HPUX_IA64_ACC \
    TRU64_ALPHA_DECCXX \
    SOLARIS_SPARC_GNU \
    SOLARIS_SPARC_CC \
    ZOS_ZSERIES_IBM \
    NSK_NONSTOP_NMCPLUS  

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

# l10n
# define the location for the compiled messages
MSG_ROOT = $(HOME_DIR)/msg

# define the location for the repository
REPOSITORY_DIR = $(HOME_DIR)
REPOSITORY_ROOT = $(REPOSITORY_DIR)/repository

# The two variables, CIM_SCHEMA_DIR and CIM_SCHEMA_VER,
# are used to control the version of the CIM Schema 
# loaded into the Pegasus Internal, InterOp,
# root/cimv2 and various test namespaces.
#
# Update the following two environment variables to 
# change the version.

# The environment variable PEGASUS_CIM_SCHEMA can be used
# to change the values of CIM_SCHEMA_DIR, CIM_SCHEMA_VER 
# and ALLOW_EXPERIMENTAL.
#
# To use the PEGASUS_CIM_SCHEMA variable the Schema mof
# files must be placed in the directory
# $(PEGAUS_ROOT)/Schemas/$(PEGASUS_CIM_SCHEMA)
# 
# The value of PEGASUS_CIM_SCHEMA must conform to the
# following syntax:
#
#        CIM[Prelim]<CIM_SCHEMA_VER>
#
# The string "Prelim" should be included if the 
# Schema contains "Experimental" class definitions.
#
# The value of <CIM_SCHEMA_VER> must be the value
# of the version string included by the DMTF as 
# part of the mof file names (e.g, CIM_Core27.mof). 
# Therefore, for example, the value of <CIM_SCHEMA_VER>
# for CIM27 Schema directories MUST be 27.
#
# Examples of valid values of PEGASUS_CIM_SCHEMA 
# include CIMPrelim27, CIM27, CIMPrelim28, and CIM28.
#
# Note the CIMPrelim271 would NOT be a valid value
# for PEGASUS_CIM_SCHEMA because the version string
# portion of the mof files (e.g., CIM_Core27.mof) in
# the CIMPrelimin271 directory is 27 not 271.

# ***** CIM_SCHEMA_DIR INFO ****
# If CIM_SCHEMA_DIR changes to use a preliminary schema which
# has experimentals make sure and change the path below to appopriate
# directory path.  Example:  CIMPrelim271 is preliminary and has
# experimental classes.  Since experimental classes exist the -aE
# option of the mof compiler needs to be set.
# *****

ifdef PEGASUS_CIM_SCHEMA
  CIM_SCHEMA_DIR=$(PEGASUS_ROOT)/Schemas/$(PEGASUS_CIM_SCHEMA)
  CIM_SCHEMA_VER=$(patsubst CIM%,%,$(patsubst CIMPrelim%,%,$(PEGASUS_CIM_SCHEMA)))
else
  CIM_SCHEMA_DIR=$(PEGASUS_ROOT)/Schemas/CIM27
  CIM_SCHEMA_VER=27
endif

ifneq (, $(findstring Prelim, $(CIM_SCHEMA_DIR)))
   ALLOW_EXPERIMENTAL = -aE
else
   ALLOW_EXPERIMENTAL =
endif

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

ifeq ($(PEGASUS_PLATFORM),LINUX_PPC_GNU)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IA64_GNU)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_ZSERIES_GNU)
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

ifeq ($(PEGASUS_PLATFORM),HPUX_IA64_ACC)
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

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
  include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
  FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
   include $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
   FOUND = true
endif

ifeq ($(PEGASUS_PLATFORM),NSK_NONSTOP_NMCPLUS)
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

################################################################################
##
##  Set up any platform independent compile conditionals by adding them to
##  precreated FLAGS parameter.
##  Assumes that the basic flags have been setup in FLAGS. 
##  Assumes that compile time flags are controlled with -D CLI option.
##
################################################################################

# Setup the conditional compile for client displays.
# 
ifdef PEGASUS_CLIENT_TRACE_ENABLE
  DEFINES+= -DPEGASUS_CLIENT_TRACE_ENABLE
endif


 
############################################################
#
# Set up other Make Variables that depend on platform config files
#
############################################################

# This is temporary until we end up with a better place to
# put this variable
# Makefiles can do directory remove with
# $(RMREPOSITORY) repositoryname
#
RMREPOSITORY = $(RMDIRHIER)




