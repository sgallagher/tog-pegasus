ROOT = ../../../../..

DIR = Pegasus/Query/QueryExpression/tests


ifneq ($(OS), HPUX) 
EXTRA_INCLUDES = -Int_includes
endif

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

LIBRARIES = \
    pegrepository \
    pegqueryexpression \
    pegwql \
    pegcql \
    pegquerycommon \
    pegconfig \
    pegcommon \

EXTRA_INCLUDES = $(SYS_INCLUDES)
#SYS_LIBS += -lfl

PROGRAM = testQueryExpression

SOURCES = QueryExpression.cpp 

QEXPRTESTNS=root/SampleProvider
QEXPRTESTNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider     

include $(ROOT)/mak/program.mak

repository:
	@ echo +++++ Loading QueryExpression Test Schema into $(QEXPRTESTNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(QEXPRTESTNS) QExprTestSchema.mof
	@ cimmofl -R$(REPOSITORY_DIR) -n$(QEXPRTESTNS) QExprTestSchemaR.mof

repositoryServer:
	@ echo +++++ Loading QueryExpression Test Schema into $(QEXPRTESTNS) namespace ...
	@ cimmof -n$(QEXPRTESTNS) QExprTestSchema.mof
	@ cimmof -n$(QEXPRTESTNS) QExprTestSchemaR.mof

tests: 
                                                                                
poststarttests:

clean:
                                                                              
install:



