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

COMPILE_COMMAND = g++

COMPILE_C_COMMAND = gcc

PLATFORM_SUFFIX = so

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

## Options:
##     -O2          turns on level 2 optimization
##     -W           prints extra warning messages
##     -Wall        enables all the warnings
##     -Wno-unused  does not print unused variable warnings
##     -fPIC        emit position-independent code
##     -Xlinker     passes an option to the linker
##       -rpath     adds a directory to the runtime library search path
#

PROGRAM_COMPILE_OPTIONS = -O2 -W -Wall -Wno-unused -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT -fPIC

PROGRAM_LINK_OPTIONS = -Xlinker -rpath -Xlinker $(PEGASUS_DEST_LIB_DIR)

LIBRARY_COMPILE_OPTIONS = $(PROGRAM_COMPILE_OPTIONS)

LIBRARY_LINK_OPTIONS = $(PROGRAM_LINK_OPTIONS)

SYS_LIBS = -ldl -lpthread -lcrypt

LIBRARY_LINK_COMMAND = $(COMPILE_COMMAND) -shared

PROGRAM_LINK_COMMAND = $(COMPILE_COMMAND)

LINK_OUT = -o
