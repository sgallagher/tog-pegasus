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

ROOT = ../../../..

DIR = Executor/tests/lib

# The source files in the Executor directory are built twice, once
# as part of the Executor executable (cimserver) and once as part
# of the a test library (pegexecutor). The object files for the
# Executor executable are placed in $PEGASUS_HOME/obj/Executor.
# To avoid conflict with these objects, the ALT_OBJ_DIR variable
# is used to define an alternative object directory for the objects
# used to build the test library.
#
# Need two more levels since ../.. will remove two levels.
ALT_OBJ_DIR = Executor/tests/lib/tests/lib

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DEXECUTOR_PLEASE_EXPORT

LIBRARY = pegexecutor

SOURCES = \
    ../../Assert.c \
    ../../Child.c \
    ../../Config.c \
    ../../Exit.c \
    ../../Fatal.c \
    ../../FileHandle.c \
    ../../Globals.c \
    ../../LocalAuth.c \
    ../../Log.c \
    ../../Macro.c \
    ../../Match.c \
    ../../Messages.c \
    ../../Options.c \
    ../../Parent.c \
    ../../PasswordFile.c \
    ../../Path.c \
    ../../Policy.c \
    ../../Process.c \
    ../../Random.c \
    ../../Socket.c \
    ../../User.c

include ../common.mak
include $(ROOT)/mak/library.mak
