ROOT = ../../../../..
DIR = Pegasus/IndicationService/tests/Subscription
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = TestSubscription

SOURCES = Subscription.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestSubscription register"
	@$(PROGRAM) register
	@$(ECHO) "TestSubscription test"
	@$(PROGRAM) test
	@$(ECHO) "TestSubscription unregister"
	@$(PROGRAM) unregister
	@$(ECHO) "+++++ TestSubscription passed all tests"
