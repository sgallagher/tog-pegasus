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
RM = rm -f

include $(ROOT)/mak/common.mak

.SUFFIXES: .xml .rsp

TMP_OBJECTS = $(foreach i,$(SOURCES),$i)

ifeq ($(OS_TYPE),windows)
CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
OBJECTS = $(CPP_OBJECTS:.c=.obj)
else
CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.o)
OBJECTS = $(CPP_OBJECTS:.c=.o)
endif

LIB = lib$(LIBRARY).$(PLATFORM_SUFFIX)

.c.o:
	$(COMPILE_C_COMMAND) -c -o $@ $(LIBRARY_COMPILE_OPTIONS) -I $(PEGASUS_INCLUDE_DIR) $(DEFINES) $*.c

.cpp.o:
	$(COMPILE_COMMAND) -c -o $@  $(LIBRARY_COMPILE_OPTIONS) -I $(PEGASUS_INCLUDE_DIR) $(DEFINES) $*.cpp

.xml.rsp:
	@ wbemexec $*.xml > $*.rsp || cd .

$(LIB): $(OBJECTS) Makefile $(ROOT)/mak/library.mak
	$(LIBRARY_LINK_COMMAND) $(LIBRARY_LINK_OPTIONS) $(DEFINES) $(LINK_OUT)$@ $(OBJECTS) $(SYS_LIBS) $(DYNAMICLIBRARIES)
	$(MAKE) -i unlink
	ln -f -s $(PEGASUS_SAMPLES_DIR)/$(SOURCE_DIR)/$(LIB) $(SYM_LINK_LIB).$(PLATFORM_SUFFIX)

rebuild:
	$(MAKE) clean
	$(MAKE)

clean:
	@$(foreach i, $(OBJECTS), $(RM) $(i);)
	$(MAKE) -i unlink
	@$(foreach i, $(LIB), $(RM) $(i);)
	@$(foreach i, $(XMLRESPONSES), $(RM) $(i);)
	@$(foreach i, $(ADDITIONAL_CLEAN_FILES), $(RM) $(i);)

XMLRESPONSES = $(XMLSCRIPTS:.xml=.rsp)
tests: $(XMLRESPONSES)

unlink:
	$(RM) $(SYM_LINK_LIB).$(PLATFORM_SUFFIX)
