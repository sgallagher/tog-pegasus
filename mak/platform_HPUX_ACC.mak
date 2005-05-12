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
MAJOR_VERSION_NUMBER = 1

OS = HPUX

ifdef ACC_COMPILER_COMMAND
   CXX = $(ACC_COMPILER_COMMAND)
else
   CXX = aCC
endif

CC = $(CXX) -Ae

COMPILER = acc

PLATFORM_VERSION_SUPPORTED = yes

SYS_INCLUDES = 

ifdef PEGASUS_CCOVER
 SYS_INCLUDES += -I/opt/ccover11/include
endif

ifdef PEGASUS_PURIFY
 SYS_INCLUDES += -I$(PURIFY_HOME)
endif

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -DPEGASUS_PLATFORM_HPUX_ACC -DPEGASUS_LOCAL_DOMAIN_SOCKET

DEFINES += -DPEGASUS_USE_SYSLOGS

DEFINES += -DPEGASUS_HAS_SIGNALS

ifdef PEGASUS_USE_EMANATE
 DEFINES += -DHPUX_EMANATE
endif

ifdef PEGASUS_CCOVER
 DEFINES += -DPEGASUS_CCOVER
endif

ifdef PEGASUS_PURIFY
 DEFINES += -DPEGASUS_PURIFY
endif

##
## The following flags need to be set or unset 
## to compile-in the code required for PAM authentication
## and compile-out the code that uses the password file.
##

ifdef PEGASUS_PAM_AUTHENTICATION
 DEFINES += -DPEGASUS_PAM_AUTHENTICATION -DPEGASUS_NO_PASSWORDFILE
endif

# l10n
ifdef PEGASUS_HAS_MESSAGES
  DEFINES += -DPEGASUS_HAS_MESSAGES
  ifdef ICU_ROOT
        MSG_COMPILE = ${ICU_INSTALL}/bin/genrb
        MSG_FLAGS =
        MSG_SOURCE_EXT = .txt
        MSG_COMPILE_EXT = .res
        CNV_ROOT_CMD = cnv2rootbundle

##################################
##
## ICU_NO_UPPERCASE_ROOT if set, specifies NOT to uppercase the root 
## resource bundle, default is to uppercase the root resource bundle^M
##
##################################

ifdef ICU_NO_UPPERCASE_ROOT
  CNV_ROOT_FLAGS =
else
  CNV_ROOT_FLAGS = -u
endif

####################################
##
## ICU_ROOT_BUNDLE_LANG if set, specifies the language that the root resource
## bundle will be generated from defaults to _en if not set.  if set, for 
## any directory containing resource bundles, there must exist a file name: 
## package(the value of ICU_ROOT_BUNDLE_LANG).txt or the make messages 
## target will fail
##
####################################

ifdef ICU_ROOT_BUNDLE_LANG
  MSG_ROOT_SOURCE = $(ICU_ROOT_BUNDLE_LANG)
else
  MSG_ROOT_SOURCE = _en
endif

    SYS_INCLUDES += -I${ICU_ROOT}/source/common
    SYS_INCLUDES += -I${ICU_ROOT}/source/i18n
    DEFINES += -DPEGASUS_HAS_ICU
    ifdef ICU_INSTALL
      EXTRA_LIBRARIES += -L${ICU_INSTALL}/lib -licui18n -licuuc
    endif
  endif
endif


DEPEND_INCLUDES =


## Flags:
##     +Z - produces position independent code (PIC).
##     +DAportable generates code for any HP9000 architecture
##     -Wl, passes the following option to the linker
##       +s causes the linked image or shared lib to be able to
##          search for any referenced shared libs dynamically in
##          SHLIB_PATH (LD_LIBRARY_PATH on 64-bit HP9000)
##       +b enables dynamic search in the specified directory(ies)
##

FLAGS = 

PEGASUS_SUPPORTS_DYNLIB=yes

ifdef PEGASUS_USE_RELEASE_DIRS
  FLAGS += -Wl,+b$(PEGASUS_DEST_LIB_DIR):/usr/lib
  ifeq ($(PEGASUS_PLATFORM), HPUX_PARISC_ACC)
    FLAGS += -Wl,+cdp,$(PEGASUS_PLATFORM_SDKROOT)/usr/lib:/usr/lib -Wl,+cdp,$(PEGASUS_HOME)/lib:$(PEGASUS_DEST_LIB_DIR)
  endif
else
  ifdef PEGASUS_HAS_MESSAGES
    ifdef ICU_ROOT
      ifdef ICU_INSTALL
        FLAGS += -Wl,+b$(LIB_DIR):/usr/lib:${ICU_INSTALL}/lib
      endif
    endif
  else
    FLAGS += -Wl,+b$(LIB_DIR):/usr/lib
  endif
endif

FLAGS += -Wl,+s

ifdef PEGASUS_DEBUG
  FLAGS += -g
endif

ifdef PEGASUS_USE_RELEASE_DIRS
    PEGASUS_DEST_LIB_DIR=/opt/wbem/lib
endif

#
#  For future use on HP-UX
#
ifdef HPUX_LARGE_INTERFACES
        FLAGS += -D_HPUX_API_LEVEL=20040821
endif

SYS_LIBS = -lpthread -lrt

SH = sh

YACC = bison

RM = rm -f

DIFF = diff

SORT = sort

COPY = cp

MOVE = mv

LIB_SUFFIX = .$(MAJOR_VERSION_NUMBER)
