ROOT = ../../..

DIR = Pegasus/Common

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_WQL_INTERNAL

EXTRA_INCLUDES = -Int_includes

LIBRARY = pegwql

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

SOURCES = \
    WQLSimplePropertySource.cpp \
    WQLPropertySource.cpp \
    WQLParserState.cpp \
    WQLSelectStatement.cpp \
    WQLOperation.cpp \
    WQLOperand.cpp \
    WQLYACC.cpp \
    WQLLEX.cpp \
    WQLParser.cpp

SOURCES_X = \
    WQLLEX.cpp \

include $(ROOT)/mak/library.mak


##
## This pushes BISON_SIMPLE into the environment so that bison will use this 
## rather than looking for /usr/local/share/bison.simple.
##

export BISON_SIMPLE = bison.simple

##
## Lex rule:
##

WQLLEX.cpp: WQL.l
	$(LEX) -oWQLLEX.cpp -PWQL_ WQL.l

##
## Yacc rule:
##

WQLYACC.cpp WQLYACC.h: WQL.y
ifeq ($(YACC),yacc)
	$(YACC) -vd -p WQL_ WQL.y
	$(COPY) y.tab.c WQLYACC.cpp
	$(COPY) y.tab.h WQLYACC.h
	$(RM) y.tab.c y.tab.h
else
	$(YACC) -vd -oWQLtemp -p WQL_ WQL.y
	$(COPY) WQLtemp WQLYACC.cpp
	$(COPY) WQLtemp.h WQLYACC.h
	$(RM) WQLtemp WQLtemp.h WQLtemp.output
endif
