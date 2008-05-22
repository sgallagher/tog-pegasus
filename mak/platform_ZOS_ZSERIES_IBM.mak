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

OS = zos

ARCHITECTURE = zseries

COMPILER = ibm

SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include -I$(ROOT)/src/StandardIncludes/zOS
#SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include
DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D _OPEN_SYS_IF_EXT -D _OPEN_SYS_DIR_EXT -D _OPEN_SYS_FILE_EXT -D_ALL_SOURCE -D_UNIX03_SOURCE -D_OPEN_THREADS=3 -D_OPEN_SYS_SOCK_IPV6 -D_ENHANCED_ASCII_EXT=0xFFFFFFFF

DEFINES += -DPEGASUS_USE_SYSLOGS

ifdef PEGASUS_KERBEROS_AUTHENTICATION
  DEFINES += -DPEGASUS_KERBEROS_AUTHENTICATION
endif



ifdef PEGASUS_USE_DEBUG_BUILD_OPTIONS
FLAGS = -W "c,debug,ASCII,XPLINK,dll,expo,langlvl(extended,newexcp),rtti(dynamiccast),float(ieee),goff"
PR_FLAGS = -W "c,debug,ASCII,XPLINK,dll,expo,langlvl(extended,newexcp),rtti(dynamiccast),FLOAT(IEEE),goff" -W "l,XPLINK,dll"
else
FLAGS = -O2 -W "c,ASCII,XPLINK,dll,expo,langlvl(extended,newexcp),rtti(dynamiccast),float(ieee),goff"
PR_FLAGS = -O2 -W "c,ASCII,XPLINK,dll,expo,langlvl(extended,newexcp),rtti(dynamiccast),FLOAT(IEEE),goff" -W "l,XPLINK,dll,EDIT=NO"
endif

ifdef PEGASUS_GENERATE_LISTINGS
  FLAGS += -W"c,LIST,XREF"
  PR_FLAGS  += -W"c,LIST,XREF" -W"l,MAP,LIST"
endif


PEGASUS_ZOS_BUILD_DATE := $(shell date '+%Y%m%d+')

ifdef PEGASUS_ZOS_SERVICE_STRING
       FLAGS += -W "c,SERVICE($(PEGASUS_ZOS_SERVICE_STRING)$(PEGASUS_ZOS_BUILD_DATE))"
    PR_FLAGS += -W "c,SERVICE($(PEGASUS_ZOS_SERVICE_STRING)$(PEGASUS_ZOS_BUILD_DATE))"
    DEFINES  += -DPEGASUS_ZOS_SERVICE_STRING=\"$(PEGASUS_ZOS_SERVICE_STRING)$(PEGASUS_ZOS_BUILD_DATE)\"
else
       FLAGS += -W "c,SERVICE(NOTVALI--CIM--NOTVALID--XXX--$(PEGASUS_ZOS_BUILD_DATE))"
    PR_FLAGS += -W "c,SERVICE(NOTVALI--CIM--NOTVALID--XXX--$(PEGASUS_ZOS_BUILD_DATE))"
endif   

# supported values for PEGASUS_ZOS_TARGET_LEVEL are the following:
# zOSV1R7, zOSV1R8, zOSV1R9
ifdef PEGASUS_ZOS_TARGET_LEVEL
       FLAGS += -W "c,TARGET($(PEGASUS_ZOS_TARGET_LEVEL))"
    PR_FLAGS += -W "c,TARGET($(PEGASUS_ZOS_TARGET_LEVEL))"
endif

ifdef PEGASUS_ZOS_SECURITY
  DEFINES += -DPEGASUS_ZOS_SECURITY
endif

ifdef PEGASUS_HAS_MY_KERBEROS
  FLAGS+= -L/usr/local/lib
  PR_FLAGS+= -L/usr/local/lib
endif

ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
  DEFINES += -DPEGASUS_ZOS_THREADLEVEL_SECURITY
endif


# if PEGASUS_ENABLE_SLP is already set then honor the users preference else
# Enable the compilation of the SLP functions.
#
ifndef PEGASUS_ENABLE_SLP
    PEGASUS_ENABLE_SLP = true
endif

SYS_LIBS =

CXX = c++ -+

CC = cc  -W "c,SSCOMM"

SH = sh

YACC = bison

RM = rm -f

DIFF = diff

SORT = sort

COPY = cp

MOVE = mv

ZIP = zip

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

PEGASUS_HAS_MAKEDEPEND = yes

ifdef PEGASUS_HAS_MESSAGES
    ifeq ($(PEGASUS_HAS_ICU),true)
        MSG_COMPILE = ${ICU_INSTALL}/bin/genrb
        MSG_FLAGS =
        MSG_SOURCE_EXT = .txt
        MSG_COMPILE_EXT = .res
        CNV_ROOT_CMD = cnv2rootbundle

        SYS_INCLUDES += -I${ICU_INSTALL}/include
        FLAGS += -L${ICU_INSTALL}/lib
        PR_FLAGS += -L${ICU_INSTALL}/lib
        EXTRA_LIBRARIES += ${ICU_INSTALL}/lib/libicui18n.x ${ICU_INSTALL}/lib/libicuuc.x
    endif
endif
