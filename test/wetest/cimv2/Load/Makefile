ROOT=../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

MOF_PATH = $(ROOT)/Schemas/CIM25

clean:
	@ $(RMDIRHIER) $(REPOSITORY_ROOT)/test#cimv2
	@ echo +++++ Loading CIM_Schema25.mof into test/cimv2 namespace...
	@ cimmofl -I$(MOF_PATH) -ntest/cimv2 $(MOF_PATH)/CIM_Schema25.mof

tests:
	
