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
## Check predefined macros:
##
##==============================================================================

ifndef __TARGET__
   $(error "__TARGET__ is undefined")
endif

ifndef __CPU__
   $(error "__CPU__ is undefined")
endif

ifndef __MINUS_T__
   $(error "__MINUS_T__ is undefined")
endif

##==============================================================================
##
## Compiler definitions:
##
##==============================================================================

COMPILER = gnu

ifdef CCACHE
  CXX = $(CCACHE) dcc
  CC = $(CCACHE) dcc
else
  CXX = dcc
  CC = dcc
endif

FLAGS += $(__MINUS_T__)
FLAGS += -Xansi
FLAGS += -Xlocal-data-area-static-only
FLAGS += -Xforce-declarations
FLAGS += -Xcode-absolute-far

# suppress "warning (etoa:4550): variable X was set but never used"
FLAGS += -ei4550

# suppress "warning (etoa:4177): variable X was declared but never referenced"
FLAGS += -ei4177

# suppress "line 471: warning (etoa:4111): statement is unreachable"
FLAGS += -ei4111


#ATTN-MEB:
PEGASUS_USE_DEBUG_BUILD_OPTIONS=1

ifdef PEGASUS_USE_DEBUG_BUILD_OPTIONS 
  FLAGS += -g -W:c:,-Xclib-optim-off
else
  FLAGS += -XO -Xsize-opt
endif

DEFINES += -DCPU=$(__CPU__)
DEFINES += -DTOOL_FAMILY=diab
DEFINES += -DTOOL=diab
DEFINES += -DPEGASUS_PLATFORM_VXWORKS_$(__CPU__)_DIAB
DEFINES += -D_WRS_KERNEL

##==============================================================================
##
## Linker definitions:
##
##==============================================================================

LINK = dld

LINK_FLAGS += $(__MINUS_T__)
LINK_FLAGS += -X 
LINK_FLAGS += -r5
LINK_FLAGS += -r4

NM = ddump -Ng

MUNCH = wtxtcl $(WIND_BASE)/host/resource/hutils/tcl/munch.tcl -c $(__TARGET__)

AR = dar
