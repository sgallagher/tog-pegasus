################################################################################
##
## Get external environment variables. Note that all external environment
## variables begin with "PEGASUS_".
##
################################################################################

ifndef ROOT
    ROOT =  $(subst \,/,$(PEGASUS_ROOT))
endif

ifdef PEGASUS_ENVVAR_FILE
    include $(PEGASUS_ENVVAR_FILE)
else
    include $(ROOT)/env_var.status
endif

ifdef PEGASUS_HOME
    HOME_DIR = $(subst \,/,$(PEGASUS_HOME))
else
    $(error PEGASUS_HOME environment variable undefined)
endif

ifdef PEGASUS_ROOT
    ROOT =  $(subst \,/,$(PEGASUS_ROOT))
else
    $(error PEGASUS_ROOT environment variable undefined)
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
    DISPLAYCONSUMER_DIR = $(subst \,/,$(PEGASUS_HOME))
endif

PLATFORM_FILES=$(wildcard $(ROOT)/mak/platform*.mak)
PLATFORM_TEMP=$(subst $(ROOT)/mak/platform_,, $(PLATFORM_FILES))
VALID_PLATFORMS=$(subst .mak,, $(PLATFORM_TEMP))

ifndef PEGASUS_PLATFORM
    $(error PEGASUS_PLATFORM environment variable undefined. Please set to\
        one of the following:$(VALID_PLATFORMS))
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
REPOSITORY_NAME = repository
REPOSITORY_ROOT = $(REPOSITORY_DIR)/$(REPOSITORY_NAME)

# define the repository mode 
#	XML = XML format
#	BIN = Binary format
#
REPOSITORY_MODE = XML

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
    ifeq ($(findstring $(patsubst CIM%,%,$(patsubst CIMPrelim%,%,$(PEGASUS_CIM_SCHEMA))),1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 271 28),)
       CIM_SCHEMA_VER=
    else
       CIM_SCHEMA_VER=$(patsubst CIM%,%,$(patsubst CIMPrelim%,%,$(PEGASUS_CIM_SCHEMA)))
    endif
else
    CIM_SCHEMA_DIR=$(PEGASUS_ROOT)/Schemas/CIM29
    CIM_SCHEMA_VER=
endif

ifneq (, $(findstring Prelim, $(CIM_SCHEMA_DIR)))
    ALLOW_EXPERIMENTAL = -aE
else
    ALLOW_EXPERIMENTAL =
endif

LEX = flex

################################################################################
##
## Default installation paths
##
################################################################################

## Default values to install files when 'make install' is invoked.

ifndef PREFIX
    PREFIX=$(HOME_DIR)/install
endif

ifndef SYSCONF_PREFIX
    SYSCONF_PREFIX=$(PREFIX)/etc
endif

ifndef LOCAL_STATE_PREFIX
    LOCAL_STATE_PREFIX=$(PREFIX)/var/
endif

ifndef DEST_LIB_DIR
    DEST_LIB_DIR = $(PREFIX)/lib
endif

ifndef DEST_BIN_DIR
    DEST_BIN_DIR = $(PREFIX)/bin
endif

ifndef DEST_SBIN_DIR
    DEST_SBIN_DIR = $(PREFIX)/sbin
endif

ifndef DEST_ETC_DIR
    DEST_ETC_DIR = $(SYSCONF_PREFIX)/pegasus
endif

ifndef DEST_MAN_DIR
    DEST_MAN_DIR = $(PREFIX)/man
endif

ifndef DEST_VAR_DIR
    DEST_VAR_DIR = $(LOCAL_STATE_PREFIX)
endif

################################################################################
##
## Attempt to include a platform configuration file:
##
################################################################################

PLATFORM_FILE = $(ROOT)/mak/platform_$(PEGASUS_PLATFORM).mak
ifneq ($(wildcard $(PLATFORM_FILE)), )
    include $(PLATFORM_FILE)
else
  $(error  PEGASUS_PLATFORM environment variable must be set to one of\
        the following:$(VALID_PLATFORMS))
endif

################################################################################
##
## Default installation macros
##
################################################################################

## INSTALL_LIB creates the destination directory if missing,
## copies the library and generates the symbolic link.

ifndef INSTALL_LIBRARY
    ## These macros are also defined in the Platform_<*>.mak files.
    INSTALL_LIBRARY =  $(MKDIRHIER) $(DEST_LIB_DIR); $(COPY) $(FULL_LIB) $(DEST_LIB_DIR)
endif

## INSTALL_PROGRAM creates the destination directory if missing and
## copies the file.
ifndef INSTALL_PROGRAM
    INSTALL_PROGRAM = $(MKDIRHIER) $(DEST_BIN_DIR); $(COPY) $(FULL_PROGRAM) $(DEST_BIN_DIR)
endif
## INSTALL_PROGRAM creates the destination directory if missing and
## copies the file.
ifndef INSTALL_SBIN_PROGRAM
    INSTALL_SBIN_PROGRAM = $(MKDIRHIER) $(DEST_SBIN_DIR);  $(COPY) $(FULL_PROGRAM) $(DEST_SBIN_DIR)
endif
## The rest of the macros for DEST_MAN_DIR, DEST_VAR_DIR, etc. are not provided in this file.

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
    DEFINES += -DPEGASUS_CLIENT_TRACE_ENABLE
endif

# do not compile trace code. sometimes it causes problems debugging
ifdef PEGASUS_REMOVE_TRACE
    DEFINES += -DPEGASUS_REMOVE_TRACE
endif

# PEP 161
# Control whether utf-8 filenames are supported by the repository
ifdef PEGASUS_SUPPORT_UTF8_FILENAME
    DEFINES += -DPEGASUS_SUPPORT_UTF8_FILENAME

    # Control whether utf-8 filenames in the repository are escaped
    ifdef PEGASUS_REPOSITORY_ESCAPE_UTF8
        DEFINES += -DPEGASUS_REPOSITORY_ESCAPE_UTF8
    endif
endif

#
# PEP 142
# The following flag need to be set to enable
# user group authorization functionality.
#
ifdef PEGASUS_ENABLE_USERGROUP_AUTHORIZATION
    DEFINES += -DPEGASUS_ENABLE_USERGROUP_AUTHORIZATION
endif

#
# PEP 193
# The following flag need to be set to disable
# CQL in indication subscriptions
#
ifdef PEGASUS_DISABLE_CQL
    DEFINES += -DPEGASUS_DISABLE_CQL
endif

#
# PEP 186
# Allow override of product name/version/status.  A file
# pegasus/src/Pegasus/Common/ProductVersion.h must exist when this
# flag is defined.
#
ifdef PEGASUS_OVERRIDE_PRODUCT_ID
    DEFINES += -DPEGASUS_OVERRIDE_PRODUCT_ID
endif

#
# PEP 197
# Allow the Provider User Context feature to be disabled.
#
ifdef PEGASUS_DISABLE_PROV_USERCTXT
    DEFINES += -DPEGASUS_DISABLE_PROV_USERCTXT
else
    ifdef PEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
        DEFINES += -DPEGASUS_DISABLE_PROV_USERCTXT_REQUESTOR
    endif
    ifdef PEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
        DEFINES += -DPEGASUS_DISABLE_PROV_USERCTXT_DESIGNATED
    endif
    ifdef PEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
        DEFINES += -DPEGASUS_DISABLE_PROV_USERCTXT_PRIVILEGED
    endif
    ifdef PEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
        DEFINES += -DPEGASUS_DISABLE_PROV_USERCTXT_CIMSERVER
    endif
endif

# PEP 211
# Controls object normalization support.
ifdef PEGASUS_ENABLE_OBJECT_NORMALIZATION
    DEFINES += -DPEGASUS_ENABLE_OBJECT_NORMALIZATION
endif

# Allow ExecQuery functionality to be enabled
ifndef PEGASUS_ENABLE_EXECQUERY
    DEFINES += -DPEGASUS_DISABLE_EXECQUERY
endif


# setup function to enable SLP functions in the Pegasus standard compile
# Set the environment varaible PEGASUS_ENABLE_SLP to enable SLP code.
# NOTE. Effective with Bug # 2633 some platforms now enable SLP.
# To see which platforms look for platform make files that set
# the variable PEGASUS_ENABLE_SLP
ifdef PEGASUS_ENABLE_SLP
    DEFINES += -DPEGASUS_ENABLE_SLP
endif

# set PEGASUS_DEBUG into the DEFINES if it exists.
# Note that this flag is the general separator between
# debug compiles and non-debug compiles and controls both
# the use of any debug options on compilers and linkers
# and general debug support that we want to be turned on in
# debug mode.
ifdef PEGASUS_DEBUG
    DEFINES += -DPEGASUS_DEBUG
endif

# compile in the experimental APIs
DEFINES += -DPEGASUS_USE_EXPERIMENTAL_INTERFACES

# Set compile flag to control compilation of CIMOM statistics
ifdef PEGASUS_DISABLE_PERFINST
    FLAGS += -DPEGASUS_DISABLE_PERFINST
endif

############################################################
#
# Set any vendor-specific compile flags
#
############################################################

ifdef PEGASUS_VENDOR_HP
    DEFINES+= -DPEGASUS_VENDOR_HP
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

ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    FLAGS += -DPEGASUS_USE_RELEASE_CONFIG_OPTIONS
endif

ifdef PEGASUS_USE_RELEASE_DIRS
    FLAGS += -DPEGASUS_USE_RELEASE_DIRS
endif

# Unless otherwise specified, Pegasus libraries go in $(PEGASUS_HOME)/lib
ifndef PEGASUS_DEST_LIB_DIR
    PEGASUS_DEST_LIB_DIR = lib
endif

ifeq ($(OS),VMS)
 DEFINES += -DPEGASUS_DEST_LIB_DIR="""$(PEGASUS_DEST_LIB_DIR)"""
else
 DEFINES += -DPEGASUS_DEST_LIB_DIR=\"$(PEGASUS_DEST_LIB_DIR)\"
endif

################################################################################
##
## Additional build flags passed in through environment variables.
## These flags are added to the compile/link commands.
##
################################################################################

ifdef PEGASUS_EXTRA_CXX_FLAGS
    EXTRA_CXX_FLAGS = $(PEGASUS_EXTRA_CXX_FLAGS)
endif

ifdef PEGASUS_EXTRA_C_FLAGS
    EXTRA_C_FLAGS = $(PEGASUS_EXTRA_C_FLAGS)
endif

ifdef PEGASUS_EXTRA_LINK_FLAGS
    EXTRA_LINK_FLAGS = $(PEGASUS_EXTRA_LINK_FLAGS)
endif

