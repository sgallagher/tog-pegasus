ROOT = ../../../..

DIR = Pegasus/Query/QueryCommon

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_QUERYCOMMON_INTERNAL -DPEGASUS_INTERNALONLY 

ifneq ($(OS), HPUX) 
EXTRA_INCLUDES = -Int_includes
endif

LIBRARY = pegquerycommon

LIBRARIES = \
   pegcommon 

SOURCES = \
    QueryContext.cpp \
    QueryException.cpp \
    QueryIdentifier.cpp \
    QueryIdentifierRep.cpp \
    QueryChainedIdentifier.cpp \
    QueryChainedIdentifierRep.cpp \
    SelectStatement.cpp \
    SelectStatementRep.cpp \
    SubRange.cpp 

include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak


