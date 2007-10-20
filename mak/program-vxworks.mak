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
## INCLUDES
##
##==============================================================================

INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

##==============================================================================
##
## _OBJECTS
##
##==============================================================================

_OBJECTS1 = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)
_OBJECTS2 = $(_OBJECTS1:.cpp=.o)
_OBJECTS = $(_OBJECTS2:.c=.o)

##==============================================================================
##
## _LIBRARIES
##
##==============================================================================

_LIBRARIES1 = $(addprefix $(LIB_DIR)/$(LIB_PREFIX), $(LIBRARIES))
_LIBRARIES2 = $(addsuffix ".a", $(_LIBRARIES1))
_LIBRARIES = $(shell echo $(_LIBRARIES2))

##==============================================================================
##
## _CTDT
##
##==============================================================================

_CTDT = $(OBJ_DIR)/ctdt.o

##==============================================================================
##
## _TARGET
##
##==============================================================================

_TARGET = $(BIN_DIR)/$(PROGRAM)

all: $(_CTDT) $(_TARGET)

$(_TARGET): $(BIN_DIR)/target $(_OBJECTS) $(_LIBRARIES) $(ERROR)
	$(CC) $(LINK_FLAGS) -o $(_TARGET) $(_CTDT) $(_OBJECTS) $(_LIBRARIES)
	@ echo "Created $(_TARGET)"

relink: clean-target
	$(MAKE) $(_TARGET)

clean-target:
	rm -f $(_TARGET)

##==============================================================================
##
## _CTDT (C++ muncher object file)
##
##==============================================================================

$(_CTDT): $(_LIBRARIES)
	$(NM) $(_LIBRARIES) | wtxtcl $(MUNCH) -c pentium > /tmp/ctdt.c
	$(CC) -c -o $(_CTDT) $(FLAGS) $(DEFINES) $(INCLUDES) /tmp/ctdt.c

##==============================================================================
##
## FILES_TO_CLEAN
##
##==============================================================================

FILES_TO_CLEAN = $(_OBJECTS) $(_TARGET) $(_CTDT)

##==============================================================================
##
## Include other makefiles:
##
##==============================================================================

include $(ROOT)/mak/objects.mak

include $(ROOT)/mak/clean.mak

-include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak
