################################################################################
##
## Get external environment variables. Note that all external environment
## variables begin with "PEGASUS_".
##
################################################################################

ifdef PEGASUS_OS_TYPE
  OS_TYPE = $(PEGASUS_OS_TYPE)
else
  OS_TYPE = windows
endif

ifdef PEGASUS_HOME
  HOME_DIR = $(subst \,/,$(PEGASUS_HOME))
else
  ERROR = pegasus_home_undefined
pegasus_home_undefined:
	@ echo PEGASUS_HOME environment variable undefined
	@ exit 1
endif

ifdef PEGASUS_ACE_ROOT
  ACE_ROOT = $(subst \,/,$(PEGASUS_ACE_ROOT))
else
  ERROR = pegasus_ace_root_undefined
pegasus_ace_root_undefined:
	@ echo PEGASUS_ACE_ROOT environment variable undefined
	@ exit 1
endif


################################################################################

OBJ_DIR = $(HOME_DIR)/obj/$(DIR)
BIN_DIR = $(HOME_DIR)/bin
LIB_DIR = $(HOME_DIR)/lib
REPOSITORY_ROOT = $(HOME_DIR)

ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/config-windows.mak
else
include $(ROOT)/mak/config-unix.mak
endif
