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
include $(ROOT)/mak/config-vms.mak

OS = VMS

ARCHITECTURE = ia64

COMPILER = deccxx

DEFINES = -DPEGASUS_OS_VMS -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

ifeq ($(PEGASUS_USE_STATIC_LIBRARIES),false)
    PEGASUS_VMS_GENERATE_EXPORT_SYMBOLS = true
endif

ifeq ($(PEGASUS_VMS_GENERATE_EXPORT_SYMBOLS),true)
    DEFINES += -DPEGASUS_VMS_GENERATE_EXPORT_SYMBOLS
endif

DEPEND_INCLUDES =

TMP_OBJDIR = $(subst ../,,$(DIR))
OBJ_DIR = $(HOME_DIR)/obj/$(TMP_OBJDIR)
BIN_DIR = $(HOME_DIR)/bin
LIB_DIR = $(HOME_DIR)/lib
OPT_DIR = $(HOME_DIR)/opt
LFLAGS =  /Threads_Enable=(Upcalls,Multiple_Kernel_Threads)
LFLAGS +=  /symbol_table=$(BIN_VMSDIRA)]$(PROGRAM)
LFLAGS +=  /map=$(BIN_VMSDIRA)]$(PROGRAM)/full/cross_reference
# run-time .dsf files -
# - do not stop in main
# - give end of file warning when scrolling through debugger window to bottom.
# - very few breakpoints to click on (none in main for example).
LFLAGS +=  /dsf=$(BIN_VMSDIRA)]$(PROGRAM)
# Add inaspefun to hide error.
#CFLAGS =  /warn=disable=(inaspefun,baddefass)
CFLAGS =  /main=POSIX_EXIT
CFLAGS += /names=(uppercase,shortened)
CFLAGS += /repos=$(CXXREPOSITORY_VMSROOT)

CCFLAGS = /main=POSIX_EXIT
#CCFLAGS = /names=(uppercase,shortened)
#CCFLAGS = /names=(as_is,shortened)
CCFLAGS += /names=(shortened)
CCFLAGS += /ieee_mode=denorm_results

ifdef PEGASUS_LIST_MACHINE_CODE
CFLAGS += /show=include/list=$(OBJ_VMSDIRA)]/debug/noopt
CCFLAGS += /show=include/list=$(OBJ_VMSDIRA)]/debug/noopt
CFLAGS += /machine_code
CCFLAGS += /machine_code
endif

ifdef PEGASUS_USE_DEBUG_BUILD_OPTIONS 
CFLAGS += /show=include/list=$(OBJ_VMSDIRA)]/debug/noopt
CCFLAGS += /show=include/list=$(OBJ_VMSDIRA)]/debug/noopt
#Do not link with /debug so that debug images can be "run"
#during the build without invoking the debugger which stops automated
#tests due to the user interface.
#Use $set image/flag=call to debug an image. 
#Note: This assumes the build is using /dsf=
#LFLAGS += /debug
else
# Note: No advantage to /debug in a release build unless
#       LFLAGS += /dsf= is used.
CFLAGS += /debug
CCFLAGS += /debug
#73-42-315 c compiler problem with /opt=inline=all
#CCFLAGS += /OPT=INLINE=ALL
CCFLAGS += /OPT=INLINE
endif

# OpenVMS 8.3-1H1 release notes section: 3.10
#CFLAGS += /pointer_size=64
#LFLAGS += /segment_attribute=code=P2

SYS_LIBS =+sys$share:sys$lib_c/lib

# SSL support
OPENSSL_VMSHOME = /Pegasus_Tools
OPENSSL_HOME = $(OPENSSL_VMSHOME)
PEGASUS_HAS_SSL = yes
OPENSSL_SET_SERIAL_SUPPORTED = true

PEGASUS_ARCHITECTURE_64BIT = yes 

#PEGASUS_ENABLE_USERGROUP_AUTHORIZATION = true
# ! PEGASUS_TEST_USER_DEFINED assumes user "pegtest"
PEGASUS_TEST_USER_DEFINED = true
#PEGASUS_TEST_USER_ID =
#PEGASUS_TEST_USER_PASS =
#PEGASUS_USE_NET_SNMP = true
#NET_SNMP_INCLUDE_DIR =
#PEGASUS_NET_SNMP_INCLUDE_DIR = USER6:[HoveyC.temp.KITS.NET-SNMP.SOURCE.NET-SNMP-5_3_1.AGENT.MIBGROUP.UCD-SNMP]
#PEGASUS_NET_SNMP_DIR =
#PEGASUS_ENABLE_PAM_STANDALONE_PROC = true
PEGASUS_ENABLE_PAM_AUTHENTICATION = true
#PEGASUS_ENABLE_REMOTE_CMPI = true
PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER = true
PEGASUS_ENABLE_SYSTEM_LOG_HANDLER = true
PEGASUS_ENABLE_EMAIL_HANDLER = true
#PEGASUS_ENABLE_JMPI_PROVIDER_MANAGER = true
#PEGASUS_ENABLE_SLP = true
#PEGASUS_DEFAULT_ENABLE_OOP = true
PEGASUS_ENABLE_COMPRESSED_REPOSITORY = true
#PEGASUS_ENABLE_IPV6 = false
PEGASUS_ENABLE_PROTOCOL_WSMAN = true

# Local domain sockets, or an equivalent, is not currently supported on OpenVMS. Bug 2147
PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET=1

# The Provider User Context feature (PEP 197) is not yet supported on OpenVMS
PEGASUS_DISABLE_PROV_USERCTXT=1

CXX = cxx

SH = sh

YACC = bison

RM = mu rm

DIFF = mu compare

SORT = mu sort

COPY = mu copy

MOVE = mu move

PEGASUS_SUPPORTS_DYNLIB = yes

LIB_SUFFIX =.olb
