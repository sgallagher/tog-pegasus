ROOT = ../../../../..
DIR = Pegasus/IndicationService/tests/DisableEnable2
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = TestDisableEnable2

SOURCES = DisableEnable2.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@echo "TestDisableEnable2 setup"
	@$(PROGRAM) setup
	@echo "TestDisableEnable2 sendFail"
	@$(PROGRAM) sendFail
	@echo "TestDisableEnable2 create"
	@$(PROGRAM) create
	@echo "TestDisableEnable2 sendSucceed"
	@$(PROGRAM) sendSucceed
	cimprovider -d -m RT_IndicationProviderModule
	@echo "TestDisableEnable2 sendBlock"
	@$(PROGRAM) sendBlock
	@echo "TestDisableEnable2 delete"
	@$(PROGRAM) delete
	@echo "TestDisableEnable2 sendBlock"
	@$(PROGRAM) sendBlock
	cimprovider -e -m RT_IndicationProviderModule
	@echo "TestDisableEnable2 sendFail"
	@$(PROGRAM) sendFail
	@echo "TestDisableEnable2 create"
	@$(PROGRAM) create
	@echo "TestDisableEnable2 sendSucceed"
	@$(PROGRAM) sendSucceed
	cimprovider -d -m RT_IndicationProviderModule
	@echo "TestDisableEnable2 sendBlock"
	@$(PROGRAM) sendBlock
	cimprovider -e -m RT_IndicationProviderModule
	@echo "TestDisableEnable2 sendSucceed"
	@$(PROGRAM) sendSucceed
	@echo "TestDisableEnable2 delete"
	@$(PROGRAM) delete
	@echo "TestDisableEnable2 sendFail"
	@$(PROGRAM) sendFail
	@echo "TestDisableEnable2 cleanup"
	@$(PROGRAM) cleanup
	@echo "+++++ TestDisableEnable2 passed all tests"
install:
