ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/IndicationStressTestProvider/testclient

include $(ROOT)/mak/config.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes
LIBRARIES = \
     pegserver \
     pegclient \
     pegrepository \
     pegconfig \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

PROGRAM = TestIndicationStressTest

SOURCES = IndicationStressTest.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestIndicationStressTest setup wql"
	@$(PROGRAM) setup wql
	@$(ECHO) "TestIndicationStressTest run 1"
	@$(PROGRAM) run 1
	@$(ECHO) "TestIndicationStressTest run 10"
	@$(PROGRAM) run 10
	@$(ECHO) "TestIndicationStressTest run 100"
	@$(PROGRAM) run 100
	@$(ECHO) "TestIndicationStressTest cleanup"
	@$(PROGRAM) cleanup cim:cql
	@$(ECHO) "TestIndicationStressTest setup cim:cql"
	@$(PROGRAM) setup cim:cql
	@$(ECHO) "TestIndicationStressTest run 1"
	@$(PROGRAM) run 1
	@$(ECHO) "TestIndicationStressTest run 10"
	@$(PROGRAM) run 10
	@$(ECHO) "TestIndicationStressTest run 100"
	@$(PROGRAM) run 100
	@$(ECHO) "TestIndicationStressTest cleanup"
	@$(PROGRAM) cleanup cim:cql
	@$(ECHO) "+++++ TestIndicationStressTest passed all tests"
	
