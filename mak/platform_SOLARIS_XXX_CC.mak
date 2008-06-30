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
#//=============================================================================

include $(ROOT)/mak/config-unix.mak

OS = solaris
COMPILER = CC

CC = cc

CXX = CC

SH = sh
YACC = yacc
RM = rm -f
DIFF = diff
SORT = sort
COPY = cp
MOVE = mv
LIB_SUFFIX = .so
PEGASUS_SUPPORTS_DYNLIB = yes
SYS_INCLUDES = 

DEFINES += -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) 
DEFINES += -D_POSIX_PTHREAD_SEMANTICS
DEFINES += -DPEGASUS_OS_SOLARIS

#
# This is needed for SPARC.  It shouldn't be needed for x86
# or x86-64 if a port is ever done for Solaris on those
# platforms.
#
DEFINES += -DTYPE_CONV

SUNOS_VERSION = $(shell uname -r)

# Pegasus requires the kernel LWP thread model.
# It doesn't exist on SunOS 5.6 or 5.7 so thery are no longer supported.
#
ifeq ($(SUNOS_VERSION), 5.6)
  DEFINES += -DSUNOS_5_6
  $(error SunOS version 5.6 is not supportted)
endif

# Pegasus requires the kernel LWP thread model.
# It doesn't exist on SunOS 5.6 or 5.7 so thery are no longer supported.
#
ifeq ($(SUNOS_VERSION), 5.7)
  DEFINES += -DSUNOS_5_7
  $(error SunOS version 5.7 is not supportted)
endif

ifeq ($(SUNOS_VERSION), 5.8)
  DEFINES += -DSUNOS_5_8
endif

ifdef PEGASUS_USE_DEBUG_BUILD_OPTIONS 
  FLAGS = -g -xs
else
  FLAGS = -O4 -s -xipo=1
endif

FLAGS += +w -KPIC -mt -xildoff

ifdef PEGASUS_SOLARIS_NEED_PTO
  FLAGS += -pto
endif

##==============================================================================
##
## COMMON_SYS_LIBS
##
##     Build the common list of libraries used in linking both libraries and
##     programs.
##
##==============================================================================

COMMON_SYS_LIBS = -lpthread -ldl -lsocket -lnsl -lxnet -lCstd

ifeq ($(SUNOS_VERSION), 5.6)
  COMMON_SYS_LIBS += -lposix4
else
  COMMON_SYS_LIBS += -lrt
endif

# on SunOS 5.8 use the alternate (kernel LWP) thread model that is standard on 
# SunOS 5.9 and 5.10
# 
ifeq ($(SUNOS_VERSION), 5.8)
  COMMON_SYS_LIBS += -R /usr/lib/lwp
endif

##==============================================================================
##
## SYS_LIBS (system libraries needed to build programs)
##
##==============================================================================

SYS_LIBS = $(COMMON_SYS_LIBS) $(EXTRA_LIBRARIES)

##==============================================================================
##
## LIBRARY_SYS_LIBS (system libraries needed to build other libraries)
##
##==============================================================================

LIBRARY_SYS_LIBS = $(COMMON_SYS_LIBS)

##==============================================================================
##
## OpenSSL Settings
##
##     To build for SSL, add PEGASUS_HAS_SSL=1 to environment
##
##==============================================================================

ifndef OPENSSL_HOME
  OPENSSL_HOME=/usr/sfw
endif

ifndef OPENSSL_BIN
  OPENSSL_BIN=/usr/sfw/bin
endif

ifndef OPENSSL_COMMAND
  OPENSSL_COMMAND=/usr/sfw/bin/openssl
endif

ifndef PEGASUS_ENABLE_SSL_CRL_VERIFICATION
  PEGASUS_ENABLE_SSL_CRL_VERIFICATION=true
endif

ifndef PEGASUS_USE_SSL_RANDOMFILE
  PEGASUS_USE_SSL_RANDOMFILE=true
endif

ifdef PEGASUS_HAS_SSL
  FLAGS += -L$(OPENSSL_HOME)/lib
endif

##==============================================================================
##
## OpenSPL Settings
##
##     To build for SSL, add PEGASUS_HAS_SSL=1 to environment
##
##==============================================================================

ifndef PEGASUS_OPENSLP_HOME
  PEGASUS_OPENSLP_HOME=/usr
endif

ifeq ($(PEGASUS_USE_OPENSLP),true)
  FLAGS += -L$(PEGASUS_OPENSLP_HOME)/lib
endif
