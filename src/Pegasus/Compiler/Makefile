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
#
#  NOTE:  The grammar and parser are no longer built in this makefile,
#  and in fact flex and bison are assumed to already have been run 
#  and their output committed to CVS.  Run 
#      make -f Makefile.grammar
#  to generate cimmof_tab.cpp, cimmof_tab.h, and cimmof_lex.cpp, 
#  and remember to  commit them afterwards.
#

ROOT = ../../..

DIR = Pegasus/Compiler

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

ifeq ($(OS_TYPE),windows)
EXTRA_INCLUDES += -I./nt_includes
endif

LOCAL_CLEAN_TARGET = local_clean_target

LOCAL_DEFINES = -DPEGASUS_COMPILER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegcompiler

LIBRARIES = \
    pegcommon \
    pegrepository \
    pegclient

PRE_DEPEND_INCLUDES = -I./depends

SOURCES = cimmof_lex_wrapper.cpp cimmofParser.cpp cimmofMessages.cpp \
        cimmofRepositoryInterface.cpp cimmofClient.cpp \
	cimmofRepository.cpp cimmof_tab_wrapper.cpp parser.cpp \
	qualifierList.cpp valueFactory.cpp objname.cpp compilerDeclContext.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak


$(OBJ_DIR)/cimmof_tab_wrapper$(OBJ) : cimmof_tab.cpp

$(OBJ_DIR)/cimmof_lex_wrapper$(OBJ) : cimmof_lex.cpp

local_clean_target:
	$(RM) cimmoftemp.out cimmoftemp.output
