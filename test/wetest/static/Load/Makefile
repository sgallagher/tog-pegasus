ROOT=../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak


clean:
	@ $(RMDIRHIER) $(REPOSITORY_ROOT)/test#static
	@ echo +++ Loading qualifier definitions into test/static +++
	@ cimmof -ntest/static $(ROOT)/test/Load/StandardQualifiers.mof
	@ echo +++ Loading PG_TestPropertyTypes schema into test/static +++
	@ cimmof -ntest/static PG_TestPropertyTypes.mof

tests:
	
