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
ifeq ($(PLATFORM_VERSION_SUPPORTED), yes)
  ifdef PLATFORM_COMPONENT_NAME
     DEFINES += -DPLATFORM_COMPONENT_NAME=\"$(PLATFORM_COMPONENT_NAME)\"
  else
     DEFINES += -DPLATFORM_COMPONENT_NAME=\"$(LIBRARY)\"
  endif
endif

INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

include $(ROOT)/mak/common.mak

################################################################################
##
## Build list of object names.
##
################################################################################

TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)

ifeq ($(OS_TYPE),windows)
CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
OBJECTS = $(CPP_OBJECTS:.c=.obj)
else
CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.o)
OBJECTS = $(CPP_OBJECTS:.c=.o)
endif

################################################################################
##
## Library rule:
##
################################################################################

ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/library-windows.mak
endif
ifeq ($(OS_TYPE),unix)
include $(ROOT)/mak/library-unix.mak
endif
ifeq ($(OS_TYPE),nsk)
include $(ROOT)/mak/library-nsk.mak
endif
ifeq ($(OS_TYPE),vms)
 include $(ROOT)/mak/library-vms.mak
endif

################################################################################
##
## Clean rules:
##
################################################################################

include $(ROOT)/mak/clean.mak

################################################################################
##
## Build list of object names:
##
################################################################################

include $(ROOT)/mak/objects.mak

include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/docxx.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak

tests: $(ERROR)

#l10n
messages: $(ERROR)

poststarttests: $(ERROR)

