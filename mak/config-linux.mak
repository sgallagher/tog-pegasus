#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
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
# Configuration options for Pegasus on all architectures running Linux

include $(ROOT)/mak/config-unix.mak

PEGASUS_PLATFORM_LINUX_GENERIC_GNU = 1
DEFINES += -DPEGASUS_PLATFORM_LINUX_GENERIC_GNU
DEFINES += -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

#########################################################################
##
## Platform specific compile options controlled by environment variables
## are set here.  
##
#########################################################################


# Enable OOP by default if preference not already set in the environment
#
ifndef PEGASUS_DEFAULT_ENABLE_OOP
PEGASUS_DEFAULT_ENABLE_OOP = true
endif


# Enable CMPI by default.
#
ifndef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER=true
endif

#########################################################################

OS = linux

COMPILER = gnu

PLATFORM_VERSION_SUPPORTED = yes

ifndef CXX
CXX = g++
endif

SH = sh

YACC = bison

RM = rm -f

DIFF = diff

SORT = sort

COPY = cp

MOVE = mv

MKDIRHIER = mkdir -p

PEGASUS_SUPPORTS_DYNLIB = yes

MAJOR_VERSION_NUMBER = 1

LIB_SUFFIX = .so.$(MAJOR_VERSION_NUMBER)

DEFINES += -DPEGASUS_USE_SYSLOGS

SYS_LIBS = -ldl -lpthread -lcrypt

# PAM support
ifdef PEGASUS_PAM_AUTHENTICATION
    SYS_LIBS += -lpam

    ## The following flags need to be set or unset
    ## to compile-in the code required for PAM authentication
    ## and compile-out the code that uses the password file.

    DEFINES += -DPEGASUS_PAM_AUTHENTICATION -DPEGASUS_NO_PASSWORDFILE
endif

FLAGS += -W -Wall -Wno-unused  -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT

##==============================================================================
##
## The DYNAMIC_FLAGS variable defines linker flags that only apply to shared
## libraries.
##
##==============================================================================
DYNAMIC_FLAGS += -fPIC

ifdef PEGASUS_USE_DEBUG_BUILD_OPTIONS 
  FLAGS += -g
else
  FLAGS += -s
  #
  # The -fno-enforce-eh-specs is not available in 2.9.5 and it probably
  # appeared in the 3.0 series of compilers.
  #
  ifeq ($(shell expr $(GCC_VERSION) '>=' 3.0), 1)
    EXTRA_CXX_FLAGS += -fno-enforce-eh-specs
  endif
  ifdef PEGASUS_OPTIMIZE_FOR_SIZE
    FLAGS += -Os
  else
    FLAGS += -O2
  endif
endif

FLAGS += $(CXX_MACHINE_OPTIONS)

ifdef PEGASUS_ENABLE_GCOV
  FLAGS += -ftest-coverage -fprofile-arcs
  SYS_LIBS += -lgcc -lgcov
  EXTRA_LIBRARIES += -lgcc -lgcov
endif

ifndef PEGASUS_USE_MU_DEPEND
PEGASUS_HAS_MAKEDEPEND = yes
endif

# l10n
ifdef PEGASUS_HAS_MESSAGES
  DEFINES += -DPEGASUS_HAS_MESSAGES
  ifeq ($(PEGASUS_HAS_ICU), true)
        MSG_COMPILE = genrb
        MSG_FLAGS =
        MSG_SOURCE_EXT = .txt
        MSG_COMPILE_EXT = .res
        CNV_ROOT_CMD = $(BIN_DIR)/cnv2rootbundle

##################################
##
## ICU_NO_UPPERCASE_ROOT if set, specifies NOT to uppercase the root resource bundle,
## default is to uppercase the root resource bundle##
##################################

ifdef ICU_NO_UPPERCASE_ROOT
  CNV_ROOT_FLAGS = 
else
  CNV_ROOT_FLAGS = -u
endif

####################################
##
##   ICU_ROOT_BUNDLE_LANG if set, specifies the language that the root resource bundle will be generated from
##   defaults to _en if not set.  if set, for any directory containing resource bundles,
##   there must exist a file name: package(the value of ICU_ROOT_BUNDLE_LANG).txt or the make messages target will fail
##
####################################

ifdef ICU_ROOT_BUNDLE_LANG
  MSG_ROOT_SOURCE = $(ICU_ROOT_BUNDLE_LANG)
else
  MSG_ROOT_SOURCE = _en
endif

    ifdef ICU_ROOT
      SYS_INCLUDES += -I${ICU_ROOT}/source/common -I${ICU_ROOT}/source/i18n
    endif
    DEFINES += -DPEGASUS_HAS_ICU
    ifdef ICU_INSTALL
      EXTRA_LIBRARIES += -L$(ICU_INSTALL)/lib
    endif
    EXTRA_LIBRARIES += -licuuc -licui18n -licudata
  endif
endif

##==============================================================================
##
## Set the default visibility symbol to hidden for shared libraries. This 
## feature is only available in GCC 4.0 and later.
##
##==============================================================================

ifeq ($(shell expr $(GCC_VERSION) '>=' 4.0), 1)
    FLAGS += -fvisibility=hidden 
endif

ifndef PEGASUS_ARCH_LIB
    ifeq ($(PEGASUS_PLATFORM),LINUX_X86_64_GNU)
        PEGASUS_ARCH_LIB = lib64
    else
        PEGASUS_ARCH_LIB = lib
    endif
endif
DEFINES += -DPEGASUS_ARCH_LIB=\"$(PEGASUS_ARCH_LIB)\"
