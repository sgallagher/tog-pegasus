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
include $(ROOT)/mak/config-unix.mak

OS = solaris

ARCHITECTURE = sparc

COMPILER = CC

CC = cc

#
# This is a hack because the Pegasus build system doesn't have a way to specify
# flags just for the C compiler or just for the C++ compiler.
#
CXX = CC -pto

SH = sh

YACC = yacc

RM = rm -f

DIFF = diff

SORT = sort

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

SYS_INCLUDES = 

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D_POSIX_PTHREAD_SEMANTICS

DEFINES += -DPEGASUS_OS_SOLARIS

DEFINES += -DPEGASUS_SNIA_INTEROP_TEST

DEFINES += -DPEGASUS_HAS_SIGNALS

#
# This is needed for SPARC.  It shouldn't be needed for x86
# or x86-64 if a port is ever done for Solaris on those
# platforms.
#
DEFINES += -DTYPE_CONV

# "READBUG" forces fstream.read to read 1 char at a time to
# overcome a bug in Wshop 6.2
# There are patches for this now.
#
# DEFINES += -DPEGASUS_OS_SOLARIS_READBUG

SUNOS_VERSION = $(shell uname -r)

ifeq ($(SUNOS_VERSION), 5.6)
DEFINES += -DSUNOS_5_6
endif

ifdef PEGASUS_DEBUG
FLAGS = -g -KPIC -mt -xs -xildoff
else
FLAGS = -O -KPIC -mt -xildoff -s -xipo=1
endif

SYS_LIBS = -lpthread -ldl -lsocket -lnsl -lxnet

ifeq ($(SUNOS_VERSION), 5.6)
SYS_LIBS += -lposix4
else
SYS_LIBS += -lrt
endif

SYS_LIBS += $(EXTRA_LIBRARIES)
