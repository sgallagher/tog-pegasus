ROOT = ../../../..

DIR = Pegasus/Query/QueryExpression

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_QUERYEXPRESSION_INTERNAL -DPEGASUS_INTERNALONLY 

ifneq ($(OS), HPUX) 
EXTRA_INCLUDES = -Int_includes
endif

LIBRARY = pegqueryexpression

LIBRARIES = \
   pegcommon \
   pegquerycommon \
   pegwql \
   pegcql 

SOURCES = \
   QueryExpression.cpp 

include $(ROOT)/mak/library.mak
include $(ROOT)/mak/install.mak


