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
VPATH = .

COMPONENTS = all
CLEAN = clean
INSTALL = install


## capture the appropriate environment variables to 
## control the build process
##
export BUILD_ROOT := /home/mdday/my-programs/pegasus/src/Unsupported/slp_client
export SRC_DIR := $(BUILD_ROOT)/src
export BUILD_DIR := $(shell pwd)
export WORDS_BIG_ENDIAN := @WORDS_BIG_ENDIAN@
export PACKAGE := slp
export PACKAGE_VERSION := 0.8 
export DEBUG := true
export LIBTOOL := /home/mdday/my-programs/pegasus/src/Unsupported/slp_client/libtool
export OBJ := .lo
export IGNORE := -
export LIB_PATH := -rpath
export LEX := flex
export YACC := bison
export STATIC_LINK_CMD := -static
export DLL_LINK_CMD := -module
export MV := mv
export MV_FLAGS := -fv
export RM := rm
export RM_FLAGS := -fv
export CP := cp
export CP_FLAGS := -fv
export MAKE := make
export OS_PORT_INC := lslp-linux.h
export OS_PORT_SRC := lslp-linux.c
export OS_PORT_OBJ := lslp-linux.lo
export HOST_OS := linux
export LINKER := gcc
export EXE := 
export LIBS := 
export THREAD_LIBS := -lpthread
export REENTRANT_FLAG := -D_REENTRANT
export NO_LINK_FLAG := -c
export LIB_SUFFIX := .o
export DLL_SUFFIX := @DLL_SUFFIX@
export EXE_OUT := -o
export OBJ_OUT := -o
export LIB_INCLUDE_FLAG := -l
export CFLAGS := -g -Wall -Dlinux -DDEBUG_ALLOC
export LDFLAGS := -g
export DLL_LIB_FLAG := 
export LFLAGS := -p -v -d 
export YFLAGS := -v -d
export bindir := /bin
export sbindir := /sbin
export libexecdir := /lib
export sysconfdir := /etc
export libdir := /usr/lib
export PLATFORM := x86
export CC := gcc

export BINDIR := /bin
export SBINDIR := /sbin
export LIBEXECDIR := /lib
export SYSCONFIGDIR := /etc
export LIBDIR := /usr/lib

all: $(COMPONENTS)
.PHONY: all

clean: $(CLEAN)
.PHONY: clean

install: $(INSTALL)
.PHONY: install

test: $(TEST)
.PHONY: test


SRC_DIRS := $(SRC_DIR) 

$(INSTALL) :
	$(foreach dir, $(SRC_DIRS), cd $(dir) && $(MAKE) all;)
	$(foreach dir, $(SRC_DIRS), cd $(dir) && $(MAKE) $(INSTALL);)
$(COMPONENTS) $(CLEAN) :
	$(foreach dir, $(SRC_DIRS),cd $(dir) && $(MAKE) $@;)


