ROOT = ../../../../..
DIR = Pegasus/IndicationService/tests/ProcessIndication
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

PROGRAM = TestProcessIndication

SOURCES = ProcessIndication.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestProcessIndication setup"
	@$(PROGRAM) setup
	@$(ECHO) "TestProcessIndication create1"
	@$(PROGRAM) create1
	@$(ECHO) "TestProcessIndication sendNormal"
	@$(PROGRAM) sendNormal
	@$(ECHO) "TestProcessIndication checkNormal"
	@$(PROGRAM) checkNormal
	@$(ECHO) "TestProcessIndication sendSubclass"
	@$(PROGRAM) sendSubclass
	@$(ECHO) "TestProcessIndication checkSubclass"
	@$(PROGRAM) checkSubclass
	@$(ECHO) "TestProcessIndication sendMissing"
	@$(PROGRAM) sendMissing
	@$(ECHO) "TestProcessIndication checkMissing"
	@$(PROGRAM) checkMissing
	@$(ECHO) "TestProcessIndication sendExtra"
	@$(PROGRAM) sendExtra
	@$(ECHO) "TestProcessIndication checkExtra"
	@$(PROGRAM) checkExtra
	@$(ECHO) "TestProcessIndication sendMatching"
	@$(PROGRAM) sendMatching
	@$(ECHO) "TestProcessIndication checkMatching"
	@$(PROGRAM) checkMatching
	@$(ECHO) "TestProcessIndication sendUnmatchingNamespace"
	@$(PROGRAM) sendUnmatchingNamespace
	@$(ECHO) "TestProcessIndication checkUnmatchingNamespace"
	@$(PROGRAM) checkUnmatchingNamespace
	@$(ECHO) "TestProcessIndication sendUnmatchingClassName"
	@$(PROGRAM) sendUnmatchingClassName
	@$(ECHO) "TestProcessIndication checkUnmatchingClassName"
	@$(PROGRAM) checkUnmatchingClassName
	@$(ECHO) "TestProcessIndication delete1"
	@$(PROGRAM) delete1
	@$(ECHO) "TestProcessIndication create2"
	@$(PROGRAM) create2
	@$(ECHO) "TestProcessIndication sendNormal"
	@$(PROGRAM) sendNormal
	@$(ECHO) "TestProcessIndication checkNormalAll"
	@$(PROGRAM) checkNormalAll
	@$(ECHO) "TestProcessIndication sendMissing"
	@$(PROGRAM) sendMissing
	@$(ECHO) "TestProcessIndication checkMissingAll"
	@$(PROGRAM) checkMissingAll
	@$(ECHO) "TestProcessIndication sendExtra"
	@$(PROGRAM) sendExtra
	@$(ECHO) "TestProcessIndication checkExtraAll"
	@$(PROGRAM) checkExtraAll
	@$(ECHO) "TestProcessIndication delete2"
	@$(PROGRAM) delete2
	@$(ECHO) "TestProcessIndication cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "+++++ TestProcessIndication passed all tests"
install:
