ROOT = ../../../..

DIR = Pegasus/CQL/CQLCLI


ifneq ($(OS), HPUX) 
EXTRA_INCLUDES = -Int_includes
endif

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

LIBRARIES = \
    pegcommon \
    pegquerycommon \
    pegcql \
    pegrepository

EXTRA_INCLUDES = $(SYS_INCLUDES)
#SYS_LIBS += -lfl

PROGRAM = cqlcli

SOURCES = CQLCLI.cpp 

CQLTESTNS=root/SampleProvider
CQLTESTNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider     

include $(ROOT)/mak/program.mak

repository:
	@ echo +++++ Loading CQL Test Schema into $(CQLTESTNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(CQLTESTNS) CQLTestSchema.mof
	@ cimmofl -n$(CQLTESTNS) CQLTestSchemaR.mof

repositoryServer:
	@ echo +++++ Loading CQL Test Schema into $(CQLTESTNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(CQLTESTNS) CQLTestSchema.mof
	@ cimmofl -n$(CQLTESTNS) CQLTestSchemaR.mof

tests:
                                                                                
poststarttests:
                                                                                
install:



