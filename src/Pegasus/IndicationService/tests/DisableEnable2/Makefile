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
	@$(ECHO) "TestDisableEnable2 setup wql"
	@$(PROGRAM) setup wql
	@$(ECHO) "TestDisableEnable2 sendFail wql"
	@$(PROGRAM) sendFail wql
	@$(ECHO) "TestDisableEnable2 create wql"
	@$(PROGRAM) create wql
	@$(ECHO) "TestDisableEnable2 sendSucceed wql"
	@$(PROGRAM) sendSucceed wql
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock wql"
	@$(PROGRAM) sendBlock wql
	@$(ECHO) "TestDisableEnable2 delete wql"
	@$(PROGRAM) delete wql
	@$(ECHO) "TestDisableEnable2 sendBlock wql"
	@$(PROGRAM) sendBlock wql
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendFail wql"
	@$(PROGRAM) sendFail wql
	@$(ECHO) "TestDisableEnable2 create wql"
	@$(PROGRAM) create wql
	@$(ECHO) "TestDisableEnable2 sendSucceed wql"
	@$(PROGRAM) sendSucceed wql
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock wql"
	@$(PROGRAM) sendBlock wql
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendSucceed wql"
	@$(PROGRAM) sendSucceed wql
	@$(ECHO) "TestDisableEnable2 delete wql"
	@$(PROGRAM) delete wql
	@$(ECHO) "TestDisableEnable2 sendFail wql"
	@$(PROGRAM) sendFail wql
	@$(ECHO) "TestDisableEnable2 cleanup wql"
	@$(PROGRAM) cleanup wql
ifndef PEGASUS_DISABLE_CQL
	@$(ECHO) "TestDisableEnable2 setup cim:cql"
	@$(PROGRAM) setup cim:cql
	@$(ECHO) "TestDisableEnable2 sendFail cim:cql"
	@$(PROGRAM) sendFail cim:cql
	@$(ECHO) "TestDisableEnable2 create cim:cql"
	@$(PROGRAM) create cim:cql
	@$(ECHO) "TestDisableEnable2 sendSucceed cim:cql"
	@$(PROGRAM) sendSucceed cim:cql
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock cim:cql"
	@$(PROGRAM) sendBlock cim:cql
	@$(ECHO) "TestDisableEnable2 delete cim:cql"
	@$(PROGRAM) delete cim:cql
	@$(ECHO) "TestDisableEnable2 sendBlock cim:cql"
	@$(PROGRAM) sendBlock cim:cql
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendFail cim:cql"
	@$(PROGRAM) sendFail cim:cql
	@$(ECHO) "TestDisableEnable2 create cim:cql"
	@$(PROGRAM) create cim:cql
	@$(ECHO) "TestDisableEnable2 sendSucceed cim:cql"
	@$(PROGRAM) sendSucceed cim:cql
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock cim:cql"
	@$(PROGRAM) sendBlock cim:cql
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendSucceed cim:cql"
	@$(PROGRAM) sendSucceed cim:cql
	@$(ECHO) "TestDisableEnable2 delete cim:cql"
	@$(PROGRAM) delete cim:cql
	@$(ECHO) "TestDisableEnable2 sendFail cim:cql"
	@$(PROGRAM) sendFail cim:cql
	@$(ECHO) "TestDisableEnable2 cleanup cim:cql"
	@$(PROGRAM) cleanup cim:cql
	@$(ECHO) "+++++ TestDisableEnable2 passed all tests"
endif

install:
