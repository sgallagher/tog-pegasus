ROOT = ../../../..

DIR = Pegasus/CQL/CQLCLI


ifneq ($(OS), HPUX) 
EXTRA_INCLUDES = -Int_includes
endif

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegprovider \
    pegcql 

EXTRA_INCLUDES = $(SYS_INCLUDES)
SYS_LIBS += -lfl

PROGRAM = cqlcli

SOURCES = CQLCLI.cpp 
     

include $(ROOT)/mak/program.mak

tests:
                                                                                
poststarttests:
                                                                                
install:



