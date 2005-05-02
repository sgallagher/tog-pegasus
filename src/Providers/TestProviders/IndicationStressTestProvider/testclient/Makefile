ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/IndicationStressTestProvider/testclient

include $(ROOT)/mak/config.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes
LIBRARIES = \
     peglistener \
     pegclient \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

PROGRAM = TestIndicationStressTest

SOURCES = IndicationStressTest.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestIndicationStressTest setup wql"
	@$(PROGRAM) setup wql
	@$(ECHO) "TestIndicationStressTest run 1"
	@$(PROGRAM) run 1
	@$(ECHO) "TestIndicationStressTest run 10 1"
	@$(PROGRAM) run 10 1
	@$(ECHO) "TestIndicationStressTest cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "TestIndicationStressTest setup cim:cql"
	@$(PROGRAM) setup cim:cql
	@$(ECHO) "TestIndicationStressTest run 1"
	@$(PROGRAM) run 1
	@$(ECHO) "TestIndicationStressTest run 10 1"
	@$(PROGRAM) run 10 1
	@$(ECHO) "TestIndicationStressTest cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "+++++ TestIndicationStressTest passed all tests"
	
