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
SYM_LINK_LIB = $(PEGASUS_PROVIDER_LIB_DIR)/lib$(LIBRARY)

COMPILE_COMMAND = aCC

COMPILE_C_COMMAND = acc

PLATFORM_SUFFIX = so

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -DHPUX_IA64_NATIVE_COMPILER

## Options:
##     +DD64  uses the LP64 data model
##     -AA    turns on newly supported ANSI C++ Standard features
##     -mt    sets various -D flags to enable multi-threading
##     -b     creates a shared library
##     -Wl,   passes the following option to the linker
##       +s   causes the linked image or shared lib to be able to
##            search for any referenced shared libs dynamically in
##            SHLIB_PATH (LD_LIBRARY_PATH on 64-bit HP9000)
##       +b   enables dynamic search in the specified directory(ies)
##     +Z     produces position independent code (PIC)
##

PROGRAM_COMPILE_OPTIONS = +DD64 -AA -mt

LIBRARY_COMPILE_OPTIONS = $(PROGRAM_COMPILE_OPTIONS) +Z

LIBRARY_LINK_OPTIONS = $(LIBRARY_COMPILE_OPTIONS) -b -Wl,+s -Wl,+b$(PEGASUS_DEST_LIB_DIR)

PROGRAM_LINK_OPTIONS = $(PROGRAM_COMPILE_OPTIONS)

SYS_LIBS = -lpthread -lrt

LIBRARY_LINK_COMMAND = $(COMPILE_COMMAND)

PROGRAM_LINK_COMMAND = $(COMPILE_COMMAND)

LINK_OUT = -o

