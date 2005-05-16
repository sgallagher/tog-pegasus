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
ROOT = ../../..

DIR = Pegasus/WQL

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_WQL_INTERNAL -DPEGASUS_INTERNALONLY

ifneq ($(OS), HPUX)
  ifneq ($(OS), zos)
    ifndef PEGASUS_LSB
	EXTRA_INCLUDES = -Int_includes
    endif
  endif
endif

LIBRARY = pegwql

LIBRARIES = \
   pegcommon \
   pegquerycommon

SOURCES = \
	WQLSelectStatement.cpp \
	WQLSelectStatementRep.cpp \
	WQLYACC.cpp \
	WQLLEX.cpp \
	WQLSimplePropertySource.cpp \
	WQLPropertySource.cpp \
	WQLInstancePropertySource.cpp \
	WQLParserState.cpp \
	WQLOperation.cpp \
	WQLOperand.cpp \
	WQLParser.cpp \
	WQLQueryExpressionRep.cpp

SOURCES_X = \
	WQLLEX.cpp \

include $(ROOT)/mak/library.mak

##------------------------------------------------------------------------------
##
## Rules to build LEX AND YACC sources:
##
## To make these files type: "make grammar"
##
##------------------------------------------------------------------------------


##
## This pushes BISON_SIMPLE into the environment so that bison will use this
## rather than looking for /usr/local/share/bison.simple.
##

grammar: yacc_rule lex_rule

export BISON_SIMPLE = bison.simple

##
## Lex rule:
##
lex_rule:
	$(LEX) -oWQLLEX.cpp -PWQL_ WQL.l
	$(TOUCH) WQLLEX.cpp

##
## Yacc rule:
##
yacc_rule:
ifeq ($(YACC),yacc)
	$(YACC) -vd -p WQL_ WQL.y
	$(COPY) "y.tab.c" "WQLYACC.cpp"
	$(COPY) "y.tab.h" "WQLYACC.h"
	$(RM) y.tab.c y.tab.h
	$(TOUCH) WQLYACC.cpp WQLYACC.h
else
	$(YACC) -vd -oWQLtemp -p WQL_ WQL.y
	$(COPY) "WQLtemp" "WQLYACC.cpp"
	$(COPY) "WQLtemp.h" "WQLYACC.h"
	$(RM) WQLtemp WQLtemp.h WQLtemp.output wqltemp.out
	$(TOUCH) WQLYACC.cpp WQLYACC.h
endif
