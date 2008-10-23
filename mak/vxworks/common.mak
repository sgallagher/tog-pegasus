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

##==============================================================================
##
## This file contains definitions common to any VxWorks platform:
##
##==============================================================================

ifndef WIND_BASE
  $(error "VxWorks environment is uninitialized (WIND_BASE undefined)")
endif

OS_TYPE = vxworks

RM = rm -f

RMDIRHIER = rm -rf

MKDIRHIER = mkdir -p 

EXE_OUT = -o

LIB_PREFIX = lib

DIFF = diff

SORT = sort

COPY = cp

TOUCH = touch

ECHO = echo

OS = vxworks

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

SYS_LIBS =

##==============================================================================
##
## DEFINES
##
##==============================================================================

DEFINES =

DEFINES += -DPEGASUS_USE_SYSLOGS 

## ATTN-KS Temporary bypass use of password file until we find way around use 
## of this file we need to support the function of persistent changable 
## passwords but not the use of the file or pam authentication
DEFINES += -DPEGASUS_NO_PASSWORDFILE

DEFINES += -DPEGASUS_REMOVE_SERVER_CLIENT_USAGE

DEFINES += -DPEGASUS_EMULATE_DYNAMIC_LOADING

##==============================================================================
##
## PEGASUS_HAS_MAKEDEPEND
##
##     Whether compilation environment has makedepend command. Otherwise,
##     use mu.
##
##==============================================================================

PEGASUS_HAS_MAKEDEPEND = yes

##==============================================================================
##
## Select Pegasus features:
##
##==============================================================================

PLATFORM_VERSION_SUPPORTED = yes

PEGASUS_DEFAULT_ENABLE_OOP = false

PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER = false

PEGASUS_ARCH_LIB = lib64

PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET=1

PEGASUS_REMOVE_TRACE=1

PEGASUS_USE_MEMORY_RESIDENT_REPOSITORY=1

PEGASUS_REMOVE_SERVER_CLIENT_USAGE=1

PEGASUS_ENABLE_IPV6=false

PEGASUS_USE_STATIC_LIBRARIES=true

PEGASUS_BUILD_CIMSERVER_LIBRARY=true

## disable the Export server in the server
## not needed or used in the vxworks environment
## today.

PEGASUS_ENABLE_EXPORTSERVER=false

ifdef PEGASUS_PAM_AUTHENTICATION
    $(error "vxworks does not support PAM authenticaiton")
endif
