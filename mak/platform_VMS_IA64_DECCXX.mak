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
include $(ROOT)/mak/config-vms.mak

OS = VMS

ARCHITECTURE = ia64

COMPILER = deccxx

SYS_VMSINCLUDES = -I$(ROOT)/src/stdcxx/cwrappers

DEFINES = -DPEGASUS_OS_VMS -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

OBJ_DIR = $(HOME_DIR)/obj/$(DIR)
BIN_DIR = $(HOME_DIR)/bin
LIB_DIR = $(HOME_DIR)/lib
OPT_DIR = $(HOME_DIR)/opt

LFLAGS = /map=$(BIN_VMSDIRA)]$(PROGRAM)
CFLAGS = /repos=$(CXXREPOSITORY_VMSROOT)/template_def=time
CCFLAGS = /OPT=INLINE=ALL/nowarn
#CFLAGS = /repos=$(CXXREPOSITORY_VMSROOT)/template_def=time/names=as_is
#CFLAGS = /repos=$(CXXREPOSITORY_VMSROOT)/template_def=time/preprocess_only/implicit_include
ifdef PEGASUS_DEBUG
CFLAGS += /debug/noopt/show=include/lis=$(OBJ_VMSDIRA)]
CCFLAGS = /debug/noopt/nowarn/show=include/lis=$(OBJ_VMSDIRA)]
#CFLAGS += /debug/noopt/show=include/lis=$(OBJ_VMSDIRA)]/be_dump="-mGLOB_show_limit_info"

LFLAGS += /debug
endif

SYS_LIBS =+sys$share:sys$lib_c/lib

# SSL support
OPENSSL_VMSHOME =/Pegasus_Blddsk/OpenSSL
OPENSSL_HOME = $(OPENSSL_VMSHOME)
PEGASUS_HAS_SSL = yes

PEGASUS_ARCHITECTURE_64BIT = yes

PEGASUS_DISABLE_PERFINST = yes
#PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER = yes

# Local domain sockets, or an equivalent, is not currently supported on OpenVMS. Bug 2147
PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET=1

CXX = cxx

SH = sh

YACC = bison

RM = mu rm

DIFF = mu diff

SORT = mu sort

COPY = mu copy

MOVE = mu move

PEGASUS_SUPPORTS_DYNLIB = yes

LIB_SUFFIX =.olb
