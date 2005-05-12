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
include $(ROOT)/mak/config-unix.mak

OS = aix

ARCHITECTURE = rs

COMPILER = xlc

PLATFORM_VERSION_SUPPORTED = yes

SYS_INCLUDES = -I/usr/vacpp/include

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -DPEGASUS_OS_AIX$(shell uname -v)

DEPEND_INCLUDES =

DEPEND_DEFINES = -D__IBMCPP__=500

ifdef PEGASUS_DEBUG
FLAGS = -g
else
FLAGS = -O2 -s
endif

FLAGS += -qrtti=dyna -qcpluscmt -qlanglvl=newexcp

SYS_LIBS = -ldl 

# SSL support
ifdef PEGASUS_HAS_SSL
ifndef OPENSSL_HOME
OPENSSL_HOME = /usr/linux
endif
endif

CXX = xlC_r

SH = sh

YACC = bison

RM = rm -f

DIFF = diff

SORT = sort

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

AIX_LIB_PRIORITY = 0
AIX_LD_HALT = 8

ifndef PEGASUS_USE_MU_DEPEND
PEGASUS_HAS_MAKEDEPEND = yes
endif

PEGASUS_SUPPORTS_DYNLIB = yes

DEFINES += -DPEGASUS_HAS_SIGNALS

# PAM support
ifdef PEGASUS_PAM_AUTHENTICATION
  EXTRA_LIBRARIES += -lpam
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
        ifdef ICU_INSTALL
          MSG_COMPILE = ${ICU_INSTALL}/bin/genrb
        else
          MSG_COMPILE = ${ICU_ROOT}/bin/genrb
        endif
        MSG_FLAGS =
        MSG_SOURCE_EXT = .txt
        MSG_COMPILE_EXT = .res
        CNV_ROOT_CMD = cnv2rootbundle

##################################
##
## ICU_NO_UPPERCASE_ROOT if set, specifies NOT to uppercase the root
## resource bundle, default is to uppercase the root resource bundle
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
        #SYS_INCLUDES += -I${ICU_ROOT}/source/common
        #SYS_INCLUDES += -I${ICU_ROOT}/source/i18n
        SYS_INCLUDES += -I${ICU_ROOT}/include
        DEFINES += -DPEGASUS_HAS_ICU
        ifdef ICU_INSTALL
          EXTRA_LIBRARIES += -L${ICU_INSTALL}/lib -licui18n -licuuc
          #SYS_LIBS += -L${ICU_INSTALL}/lib -licui18n -licuuc
        endif
  endif
endif

ifdef PEGASUS_USE_RELEASE_DIRS
  PEGASUS_DEST_LIB_DIR =/usr/lib
endif

