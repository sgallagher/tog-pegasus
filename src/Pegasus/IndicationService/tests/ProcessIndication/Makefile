ROOT = ../../../../..
DIR = Pegasus/IndicationService/tests/ProcessIndication
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

PROGRAM = TestProcessIndication

SOURCES = ProcessIndication.cpp

ifeq ($(OS),VMS)
LOCAL_DEFINES = -DINDICATION_DIR="""$(DISPLAYCONSUMER_DIR)""" -DPEGASUS_INTERNALONLY
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestProcessIndication setup wql"
	@$(PROGRAM) setup wql
	@$(ECHO) "TestProcessIndication create1 wql"
	@$(PROGRAM) create1 wql
	@$(ECHO) "TestProcessIndication sendNormal wql"
	@$(PROGRAM) sendNormal wql
	@$(ECHO) "TestProcessIndication checkNormal wql"
	@$(PROGRAM) checkNormal wql
	@$(ECHO) "TestProcessIndication sendSubclass wql"
	@$(PROGRAM) sendSubclass wql
	@$(ECHO) "TestProcessIndication checkSubclass wql"
	@$(PROGRAM) checkSubclass wql
	@$(ECHO) "TestProcessIndication sendMissing wql"
	@$(PROGRAM) sendMissing wql
	@$(ECHO) "TestProcessIndication checkMissing wql"
	@$(PROGRAM) checkMissing wql
	@$(ECHO) "TestProcessIndication sendExtra wql"
	@$(PROGRAM) sendExtra wql
	@$(ECHO) "TestProcessIndication checkExtra wql"
	@$(PROGRAM) checkExtra wql
	@$(ECHO) "TestProcessIndication sendMatching wql"
	@$(PROGRAM) sendMatching wql
	@$(ECHO) "TestProcessIndication checkMatching wql"
	@$(PROGRAM) checkMatching wql
	@$(ECHO) "TestProcessIndication sendUnmatchingNamespace wql"
	@$(PROGRAM) sendUnmatchingNamespace wql
	@$(ECHO) "TestProcessIndication checkUnmatchingNamespace wql"
	@$(PROGRAM) checkUnmatchingNamespace wql
	@$(ECHO) "TestProcessIndication sendUnmatchingClassName wql"
	@$(PROGRAM) sendUnmatchingClassName wql
	@$(ECHO) "TestProcessIndication checkUnmatchingClassName wql"
	@$(PROGRAM) checkUnmatchingClassName wql
	@$(ECHO) "TestProcessIndication delete1 wql"
	@$(PROGRAM) delete1 wql
	@$(ECHO) "TestProcessIndication create2 wql"
	@$(PROGRAM) create2 wql
	@$(ECHO) "TestProcessIndication sendNormal wql"
	@$(PROGRAM) sendNormal wql
	@$(ECHO) "TestProcessIndication checkNormalAll wql"
	@$(PROGRAM) checkNormalAll wql
	@$(ECHO) "TestProcessIndication sendMissing wql"
	@$(PROGRAM) sendMissing wql
	@$(ECHO) "TestProcessIndication checkMissingAll wql"
	@$(PROGRAM) checkMissingAll wql
	@$(ECHO) "TestProcessIndication sendExtra wql"
	@$(PROGRAM) sendExtra wql
	@$(ECHO) "TestProcessIndication checkExtraAll wql"
	@$(PROGRAM) checkExtraAll wql
	@$(ECHO) "TestProcessIndication delete2 wql"
	@$(PROGRAM) delete2 wql
	@$(ECHO) "TestProcessIndication cleanup wql"
	@$(PROGRAM) cleanup wql
	@$(ECHO) "TestProcessIndication setup cim:cql"
	@$(PROGRAM) setup cim:cql
	@$(ECHO) "TestProcessIndication create1 cim:cql"
	@$(PROGRAM) create1 cim:cql
	@$(ECHO) "TestProcessIndication sendNormal cim:cql"
	@$(PROGRAM) sendNormal cim:cql
	@$(ECHO) "TestProcessIndication checkNormal cim:cql"
	@$(PROGRAM) checkNormal cim:cql
	@$(ECHO) "TestProcessIndication sendSubclass cim:cql"
	@$(PROGRAM) sendSubclass cim:cql
	@$(ECHO) "TestProcessIndication checkSubclass cim:cql"
	@$(PROGRAM) checkSubclass cim:cql
	@$(ECHO) "TestProcessIndication sendMissing cim:cql"
	@$(PROGRAM) sendMissing cim:cql
	@$(ECHO) "TestProcessIndication checkMissing cim:cql"
	@$(PROGRAM) checkMissing cim:cql
	@$(ECHO) "TestProcessIndication sendExtra cim:cql"
	@$(PROGRAM) sendExtra cim:cql
	@$(ECHO) "TestProcessIndication checkExtra cim:cql"
	@$(PROGRAM) checkExtra cim:cql
	@$(ECHO) "TestProcessIndication sendMatching cim:cql"
	@$(PROGRAM) sendMatching cim:cql
	@$(ECHO) "TestProcessIndication checkMatching cim:cql"
	@$(PROGRAM) checkMatching cim:cql
	@$(ECHO) "TestProcessIndication sendUnmatchingNamespace cim:cql"
	@$(PROGRAM) sendUnmatchingNamespace cim:cql
	@$(ECHO) "TestProcessIndication checkUnmatchingNamespace cim:cql"
	@$(PROGRAM) checkUnmatchingNamespace cim:cql
	@$(ECHO) "TestProcessIndication sendUnmatchingClassName cim:cql"
	@$(PROGRAM) sendUnmatchingClassName cim:cql
	@$(ECHO) "TestProcessIndication checkUnmatchingClassName cim:cql"
	@$(PROGRAM) checkUnmatchingClassName cim:cql
	@$(ECHO) "TestProcessIndication delete1 cim:cql"
	@$(PROGRAM) delete1 cim:cql
	@$(ECHO) "TestProcessIndication create2 cim:cql"
	@$(PROGRAM) create2 cim:cql 
	@$(ECHO) "TestProcessIndication sendNormal cim:cql"
	@$(PROGRAM) sendNormal cim:cql
	@$(ECHO) "TestProcessIndication checkNormalAll cim:cql"
	@$(PROGRAM) checkNormalAll cim:cql
	@$(ECHO) "TestProcessIndication sendMissing cim:cql"
	@$(PROGRAM) sendMissing cim:cql
	@$(ECHO) "TestProcessIndication checkMissingAll cim:cql"
	@$(PROGRAM) checkMissingAll cim:cql
	@$(ECHO) "TestProcessIndication sendExtra cim:cql"
	@$(PROGRAM) sendExtra cim:cql
	@$(ECHO) "TestProcessIndication checkExtraAll cim:cql"
	@$(PROGRAM) checkExtraAll cim:cql
	@$(ECHO) "TestProcessIndication delete2 cim:cql"
	@$(PROGRAM) delete2 cim:cql
	@$(ECHO) "TestProcessIndication create3 cim:cql"
	@$(PROGRAM) create3 cim:cql
	@$(ECHO) "TestProcessIndication sendNormal cim:cql"
	@$(PROGRAM) sendNormal cim:cql
	@$(ECHO) "TestProcessIndication checkNormalWhere cim:cql"
	@$(PROGRAM) checkNormalWhere cim:cql
	@$(ECHO) "TestProcessIndication sendMissing cim:cql"
	@$(PROGRAM) sendMissing cim:cql
	@$(ECHO) "TestProcessIndication checkMissingWhere cim:cql"
	@$(PROGRAM) checkMissingWhere cim:cql
	@$(ECHO) "TestProcessIndication delete3 cim:cql"
	@$(PROGRAM) delete3 cim:cql
	@$(ECHO) "TestProcessIndication create4 cim:cql"
	@$(PROGRAM) create4 cim:cql
	@$(ECHO) "TestProcessIndication sendNormal cim:cql"
	@$(PROGRAM) sendNormal cim:cql
	@$(ECHO) "TestProcessIndication checkNormalWhereNotSatisfied cim:cql"
	@$(PROGRAM) checkNormalWhereNotSatisfied cim:cql
	@$(ECHO) "TestProcessIndication sendMissing cim:cql"
	@$(PROGRAM) sendMissing cim:cql
	@$(ECHO) "TestProcessIndication checkMissingWhere cim:cql"
	@$(PROGRAM) checkMissingWhere cim:cql
	@$(ECHO) "TestProcessIndication delete4 cim:cql"
	@$(PROGRAM) delete4 cim:cql
	@$(ECHO) "TestProcessIndication cleanup cim:cql"
	@$(PROGRAM) cleanup cim:cql
	@$(ECHO) "+++++ TestProcessIndication passed all tests"

IndInit_TS1a:
	@$(ECHO) "TestProcessIndication setup wql"
	@$(PROGRAM) setup wql
	@$(ECHO) "TestProcessIndication create1 wql"
	@$(PROGRAM) create1 wql
	@$(ECHO) "TestProcessIndication create2 wql"
	@$(PROGRAM) create2 wql

IndInit_TS1b:
	@$(ECHO) "TestProcessIndication delete1 wql"
	@$(PROGRAM) delete1 wql
	@$(ECHO) "TestProcessIndication delete2 wql"
	@$(PROGRAM) delete2 wql
	@$(ECHO) "TestProcessIndication cleanup wql"
	@$(PROGRAM) cleanup wql
