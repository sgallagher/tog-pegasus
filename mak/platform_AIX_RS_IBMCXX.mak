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
include $(ROOT)/mak/config-unix.mak

OS = aix

ARCHITECTURE = rs

COMPILER = xlc

PLATFORM_VERSION_SUPPORTED = yes

SYS_INCLUDES = -I/usr/vacpp/include

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -DPEGASUS_AIX_VERSION=$(shell uname -v)

DEPEND_INCLUDES =

DEPEND_DEFINES = -D__IBMCPP__=500

ifdef PEGASUS_USE_DEBUG_BUILD_OPTIONS 
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

ifdef PEGASUS_USE_RELEASE_DIRS
  PEGASUS_DEST_LIB_DIR =/usr/lib
endif

ifdef PEGASUS_ENABLE_JMPI_PROVIDER_MANAGER
  JVM_LINK_FLAGS=-Wl,-bM:UR
endif
