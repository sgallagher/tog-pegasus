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

ifndef __TARGET__
   $(error "__TARGET__ is undefined")
endif

ifndef __CPU__
   $(error "__CPU__ is undefined")
endif

COMPILER = gnu

ifdef CCACHE
  CXX = $(CCACHE) c++$(__TARGET__)
  CC = $(CCACHE) cc$(__TARGET__)
else
  CXX = c++$(__TARGET__)
  CC = cc$(__TARGET__)
endif

NM = nm$(__TARGET__)

FLAGS += -ansi
FLAGS += -W
FLAGS += -Wall
FLAGS += -Wno-unused

ifdef PEGASUS_USE_DEBUG_BUILD_OPTIONS 
  FLAGS += -g
else
  FLAGS += -O2
endif

#ifeq ($(shell expr $(GCC_VERSION) '>=' 3.0), 1)
#  FLAGS += -fno-enforce-eh-specs
#endif

DEFINES += -DCPU=$(__CPU__)
DEFINES += -DTOOL_FAMILY=gnu 
DEFINES += -DTOOL=gnu 
DEFINES += -DPEGASUS_PLATFORM_VXWORKS_$(__CPU__)_GNU

MUNCH = wtxtcl $(WIND_BASE)/host/resource/hutils/tcl/munch.tcl -c $(__TARGET__)

LINK_FLAGS += -r 
LINK_FLAGS += -nostdlib 
LINK_FLAGS += -Wl,-X
LINK_FLAGS += $(WIND_BASE)/target/h/tool/gnu/ldscripts/link.OUT
