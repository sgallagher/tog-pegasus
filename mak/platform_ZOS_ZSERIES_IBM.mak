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

OS = zos

ARCHITECTURE = zseries

COMPILER = ibm

#SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include -I$(ROOT)/src/StandardIncludes/zOS
SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include
DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D_OPEN_SOURCE=3

ifdef PEGASUS_KERBEROS_AUTHENTICATION
  DEFINES += -DPEGASUS_KERBEROS_AUTHENTICATION
endif


DEPEND_DEFINES = -D__IBMCPP__=400

ifdef PEGASUS_DEBUG
FLAGS = -g -W "c,ASCII,XPLINK,dll,expo,rtti(dynamiccast),langlvl(extended),float(ieee)"
PR_FLAGS = -g -W "c,ASCII,XPLINK,dll,expo,langlvl(extended),FLOAT(IEEE)" -W "l,XPLINK,dll"
else
FLAGS = -O2 -W "c,ASCII,XPLINK,dll,expo,rtti(dynamiccast),langlvl(extended),float(ieee)"
PR_FLAGS = -O2 -W "c,ASCII,XPLINK,dll,expo,langlvl(extended),FLOAT(IEEE)" -W "l,XPLINK,dll"
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

# Local domain sockets, or an equivalent, is not currently supported on z/OS. Bug 2147
PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET=1

SYS_LIBS =

CXX = c++ -+

CC = cc

SH = sh

YACC = bison

RM = rm -f

DIFF = diff

SORT = sort

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

PEGASUS_HAS_MAKEDEPEND = yes
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
        SYS_INCLUDES += -I${ICU_INSTALL}/include
        DEFINES += -DPEGASUS_HAS_ICU
        ifdef ICU_INSTALL
	  FLAGS+ = -L${ICU_INSTALL}/lib
	  PR_FLAGS += -L${ICU_INSTALL}/lib
          EXTRA_LIBRARIES += ${ICU_INSTALL}/lib/libicui18n.x ${ICU_INSTALL}/lib/libicuuc.x
        endif
  endif
endif
