ROOT = ../../..

DIR = Pegasus/CQL

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CQL_INTERNAL -DPEGASUS_INTERNALONLY -DCQLINPUT

ifneq ($(OS), HPUX) 
EXTRA_INCLUDES = -Int_includes
endif

LIBRARY = pegcql

LIBRARIES = \
   pegcommon 

SOURCES = \
   CQLYACC.cpp \
   CQLLEX.cpp \
   CQLChainedIdentifier.cpp \
   CQLChainedIdentifierRep.cpp \
   CQLIdentifier.cpp \
   CQLIdentifierRep.cpp \
   SubRange.cpp \
   QueryContext.cpp \
   CIMOMHandleQueryContext.cpp \
   RepositoryQueryContext.cpp \
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
   SelectStatement.cpp \
   SelectStatementRep.cpp \
   CQLValue.cpp \
   CQLValueRep.cpp \
   QueryException.cpp \
   CQLUtilities.cpp

include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak


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
