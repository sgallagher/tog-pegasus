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

ifndef WIND_BASE
  $(error "The WIND_BASE environment variable is undefined")
endif

export PEGASUS_ENABLE_IPV6=false

DEFINES = 

OS_TYPE = vxworks

RM = rm -f

RMDIRHIER = rm -rf

MKDIRHIER = mkdir -p 

EXE_OUT = -o

OBJ = .o

OBJ_OUT = -o

EXE =

LIB_PREFIX = lib

DIFF = diff

SORT = sort

COPY = cp

TOUCH = touch

ECHO = echo

DEFINES += -DPEGASUS_PLATFORM_VXWORKS_PENTIUM_GNU

PEGASUS_DEFAULT_ENABLE_OOP = false

PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER = false

OS = vxworks

COMPILER = gnu

PLATFORM_VERSION_SUPPORTED = yes

ifndef CXX
  CXX = c++pentium
endif

ifndef CC
  CC = ccpentium
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

LIB_SUFFIX = .so

DEFINES += -DPEGASUS_USE_SYSLOGS -DCPU=SIMPENTIUM -DTOOL_FAMILY=gnu -DTOOL=gnu 

SYS_LIBS =

ifdef PEGASUS_PAM_AUTHENTICATION
    $(error "vxworks does not support PAM authenticaiton")
endif

FLAGS += -mcpu=i486 -march=i486 -W -Wall -Wno-unused -mrtp -MD -MP

ifdef PEGASUS_USE_DEBUG_BUILD_OPTIONS 
  FLAGS += -g
else
  FLAGS += -Os
endif

PEGASUS_HAS_MAKEDEPEND = yes

PEGASUS_ARCH_LIB = lib64

VXWORKS_LIB_COMMON = $(WIND_BASE)/target/usr/lib/simpentium/SIMPENTIUM/common

ROMFS = $(PEGASUS_HOME)/romfs
