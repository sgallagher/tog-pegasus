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

DIR = Pegasus/CQL

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CQL_INTERNAL -DPEGASUS_INTERNALONLY -DCQLINPUT  

# Next line can be used to debug CQL.y
#LOCAL_DEFINES = -DPEGASUS_CQL_INTERNAL -DPEGASUS_INTERNALONLY -DCQL_DEBUG_GRAMMAR -DCQLINPUT 

ifneq ($(OS), HPUX) 
ifneq ($(OS), zos)
EXTRA_INCLUDES = -Int_includes
endif
endif

LIBRARY = pegcql

LIBRARIES = \
    pegquerycommon \
    pegcommon 

SOURCES = \
   CQLYACC.cpp \
   CQLLEX.cpp \
   CQLChainedIdentifier.cpp \
   CQLChainedIdentifierRep.cpp \
   CQLIdentifier.cpp \
   CQLIdentifierRep.cpp \
   CQLParser.cpp \
   CQLParserState.cpp \
   CQLSimplePredicate.cpp \
   CQLSimplePredicateRep.cpp \
   CQLPredicate.cpp \
   CQLPredicateRep.cpp \
   CQLTerm.cpp \
   CQLTermRep.cpp \
   CQLFunction.cpp \
   CQLFunctionRep.cpp \
   CQLExpression.cpp \
   CQLExpressionRep.cpp \
   CQLFactor.cpp \
   CQLFactorRep.cpp \
   CQLFactory.cpp \
   CQLSelectStatement.cpp \
   CQLSelectStatementRep.cpp \
   CQLValue.cpp \
   CQLValueRep.cpp \
   CQLRegularExpression.cpp \
   CQLUtilities.cpp \
   Cql2Dnf.cpp 

include $(ROOT)/mak/library.mak


##-----------------------------------------------------------------------------
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
#YACC = bison                                                                                                                                       
##
## Lex rule:
##
lex_rule:
	$(LEX) -PCQL_ CQL.l
	$(COPY) lex.CQL_.c CQLLEX.cpp
	$(RM) lex.CQL_.c
	$(TOUCH) CQLLEX.cpp

##
## Yacc rule:
##
yacc_rule:
ifeq ($(YACC),yacc)
	$(YACC) -vd -p CQL_ CQL.y
	$(COPY) y.tab.c CQLYACC.cpp
	$(COPY) y.tab.h CQLYACC.h
	$(RM) y.tab.c y.tab.h 
	$(TOUCH) CQLYACC.cpp
else
	$(YACC) -vd -oCQLtemp -p CQL_ CQL.y
	$(COPY) CQLtemp CQLYACC.cpp
	$(COPY) CQLtemp.h CQLYACC.h
	$(RM) CQLtemp CQLtemp.h
	$(TOUCH) CQLYACC.cpp CQLYACC.h
endif

