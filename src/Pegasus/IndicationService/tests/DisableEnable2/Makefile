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
	@$(ECHO) "TestDisableEnable2 setup"
	@$(PROGRAM) setup
	@$(ECHO) "TestDisableEnable2 sendFail"
	@$(PROGRAM) sendFail
	@$(ECHO) "TestDisableEnable2 create"
	@$(PROGRAM) create
	@$(ECHO) "TestDisableEnable2 sendSucceed"
	@$(PROGRAM) sendSucceed
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock"
	@$(PROGRAM) sendBlock
	@$(ECHO) "TestDisableEnable2 delete"
	@$(PROGRAM) delete
	@$(ECHO) "TestDisableEnable2 sendBlock"
	@$(PROGRAM) sendBlock
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendFail"
	@$(PROGRAM) sendFail
	@$(ECHO) "TestDisableEnable2 create"
	@$(PROGRAM) create
	@$(ECHO) "TestDisableEnable2 sendSucceed"
	@$(PROGRAM) sendSucceed
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock"
	@$(PROGRAM) sendBlock
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendSucceed"
	@$(PROGRAM) sendSucceed
	@$(ECHO) "TestDisableEnable2 delete"
	@$(PROGRAM) delete
	@$(ECHO) "TestDisableEnable2 sendFail"
	@$(PROGRAM) sendFail
	@$(ECHO) "TestDisableEnable2 cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "+++++ TestDisableEnable2 passed all tests"

install:
