ROOT = ../../../../..
DIR = Pegasus/IndicationService/tests/DisableEnable
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = TestDisableEnable

SOURCES = DisableEnable.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestDisableEnable setup wql"
	@$(PROGRAM) setup wql
	@$(ECHO) "TestDisableEnable create wql"
	@$(PROGRAM) create wql
	cimprovider -d -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete wql"
	@$(PROGRAM) delete wql
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable create wql"
	@$(PROGRAM) create wql
	cimprovider -d -m ProcessIndicationProviderModule
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete wql"
	@$(PROGRAM) delete wql
	@$(ECHO) "TestDisableEnable create wql"
	@$(PROGRAM) create wql
	@$(ECHO) "TestDisableEnable create2 wql"
	@$(PROGRAM) create2 wql
	@$(ECHO) "TestDisableEnable addProvider wql"
	@$(PROGRAM) addProvider wql
	@$(ECHO) "TestDisableEnable modifyProvider1 wql"
	@$(PROGRAM) modifyProvider1 wql
	@$(ECHO) "TestDisableEnable modifyProvider2 wql"
	@$(PROGRAM) modifyProvider2 wql
	@$(ECHO) "TestDisableEnable deleteProvider wql"
	@$(PROGRAM) deleteProvider wql
	@$(ECHO) "TestDisableEnable delete2 wql"
	@$(PROGRAM) delete2 wql
	@$(ECHO) "TestDisableEnable delete wql"
	@$(PROGRAM) delete wql
	@$(ECHO) "TestDisableEnable cleanup wql"
	@$(PROGRAM) cleanup wql
ifndef PEGASUS_DISABLE_CQL
	@$(ECHO) "TestDisableEnable setup cim:cql"
	@$(PROGRAM) setup cim:cql
	@$(ECHO) "TestDisableEnable create cim:cql"
	@$(PROGRAM) create cim:cql
	cimprovider -d -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete cim:cql"
	@$(PROGRAM) delete cim:cql
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable create cim:cql"
	@$(PROGRAM) create cim:cql
	cimprovider -d -m ProcessIndicationProviderModule
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete cim:cql"
	@$(PROGRAM) delete cim:cql
	@$(ECHO) "TestDisableEnable create cim:cql"
	@$(PROGRAM) create cim:cql
	@$(ECHO) "TestDisableEnable create2 cim:cql"
	@$(PROGRAM) create2 cim:cql
	@$(ECHO) "TestDisableEnable addProvider cim:cql"
	@$(PROGRAM) addProvider cim:cql
	@$(ECHO) "TestDisableEnable modifyProvider1 cim:cql"
	@$(PROGRAM) modifyProvider1 cim:cql
	@$(ECHO) "TestDisableEnable modifyProvider2 cim:cql"
	@$(PROGRAM) modifyProvider2 cim:cql
	@$(ECHO) "TestDisableEnable deleteProvider cim:cql"
	@$(PROGRAM) deleteProvider cim:cql
	@$(ECHO) "TestDisableEnable delete2 cim:cql"
	@$(PROGRAM) delete2 cim:cql
	@$(ECHO) "TestDisableEnable delete cim:cql"
	@$(PROGRAM) delete cim:cql
	@$(ECHO) "TestDisableEnable cleanup cim:cql"
	@$(PROGRAM) cleanup cim:cql
	@$(ECHO) "+++++ TestDisableEnable passed all tests"
endif
