ROOT=../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak
include $(ROOT)/test/config.mak


clean:
	@ $(RMDIRHIER) $(REPOSITORY_ROOT)/test#static
	@ echo +++ Loading qualifier definitions into test/static +++
	@ cimmofl -ntest/static $(ROOT)/test/Load/StandardQualifiers.mof
	@ echo +++ Loading PG_TestPropertyTypes schema into test/static +++
	@ cimmofl -ntest/static PG_TestPropertyTypes.mof
	@ echo +++ Register PG_TestPropertyTypes schema into $(INTEROPNS) +++
	@ cimmofl -n$(INTEROPNS) PG_TestPropertyTypesR.mof

tests:
	
