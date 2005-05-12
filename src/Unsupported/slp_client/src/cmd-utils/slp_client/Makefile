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
PROJECT = slp_client
LFLAGS += -o$@ 
YFLAGS += -o$@ 

SOURCES = slp_client.c \
	$(SRC_DIR)/$(OS_PORT_SRC) \
	y_attr.c \
	l_attr.c \
	y_url.c \
	l_url.c \
	y_filter.c \
	l_filter.c 	

OBJS = slp_client$(OBJ) \
	$(SRC_DIR)/$(OS_PORT_OBJ) \
	y_attr$(OBJ) \
	l_attr$(OBJ) \
	y_url$(OBJ) \
	l_url$(OBJ) \
	y_filter$(OBJ) \
	l_filter$(OBJ) 

VERSION = 1:0:0

all: lib$(PROJECT).la
.PHONY: all

clean: $(CLEAN) clean.txt
.PHONY: clean


install: $(INSTALL) install.txt
.PHONY: install

remove: $(REMOVE) remove.txt
.PHONY: remove

lib$(PROJECT).la: $(OBJS)
	$(LIBTOOL) $(CC) $(LIB_PATH) /usr/local/lib -version-info $(VERSION) $(LDFLAGS) $(DLL_LIB_FLAG) $(OBJ_OUT) $@ $(OBJS) $(LIBS)

%$(OBJ) : %.c
	$(LIBTOOL) $(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) $< 

$(SRC_DIR)/$(OS_PORT_OBJ) : $(SRC_DIR)/$(OS_PORT_SRC) 
	$(LIBTOOL) $(CC) $(NO_LINK_FLAG) $(CFLAGS) $(DEFS) $(OBJ_OUT) $@ $< 

l_attr.c : attr.l
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_attr.c : attr.y 
	$(YACC) -o$@ -p$(basename $<) -v -d $<
l_url.c : url.l
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_url.c : url.y
	$(YACC) -o$@ -p$(basename $<) -v -d $<
l_filter.c : filter.l 
	$(LEX) -p -v -o$@ -P$(basename $<) $<
y_filter.c : filter.y
	$(YACC) -o$@ -p$(basename $<) -v -d $<

clean.txt :
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) .libs/*
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *.o
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) l_*.c 
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) y_*.c 
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *.lo
install.txt: 
	$(LIBTOOL)  $(CP) $(CP_FLAGS) libslp_client.* /usr/local/lib

remove.txt: 
	$(LIBTOOL) $(RM) $(RM_FLAGS) /usr/local/lib/libslp_client.*









