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
PROJECT = slp_query
LFLAGS += -o$@
YFLAGS += -o$@

SOURCES = slp_query.c 

OBJS = slp_query$(OBJ) 

LIBS = $(SRC_CLIENT)/libslp_client.la

all: $(PROJECT)$(EXE)
.PHONY: all

install: $(INSTALL) install.txt
.PHONY: install

remove: $(REMOVE) remove.txt
.PHONY: remove

clean: $(CLEAN) clean.txt
.PHONY: clean


$(PROJECT)$(EXE): slp_query$(OBJ)
	$(LIBTOOL) $(CC) $(LDFLAGS) $(EXE_OUT) $@ $(OBJS) $(LIBS)

slp_query$(OBJ) : slp_query.c 
	$(LIBTOOL) $(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) $< $(OBJ_OUT) $@

clean.txt :
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) $(SUB_PROJECT)$(EXE)
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *.o
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *.lo

install.txt: $(PROJECT)$(EXE)
	$(IGNORE)$(LIBTOOL) $(CP) $(CP_FLAGS) slp_query /usr/local/bin

remove.txt:
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) /usr/local/bin/slp_query







