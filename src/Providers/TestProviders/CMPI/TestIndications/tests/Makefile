ROOT = ../../../../../../
DIR = Providers/TestProviders/CMPI/TestIndications/tests

include $(ROOT)/mak/config.mak

LIBRARIES = \
     pegserver \
     pegclient \
     pegrepository \
     pegconfig \
     pegcommon

LOGFILES = TestCMPIIndication

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = TestCMPIIndicationSubscription

SOURCES = TestCMPIIndicationSubscription.cpp

include $(ROOT)/mak/program.mak
include $(ROOT)/test/config.mak

tests:

poststarttests:
	@$(RM) $(LOGFILES).log
	@$(ECHO) $(PROGRAM) test
	@$(PROGRAM) test
	@$(ECHO) +++ $(PROGRAM) passed successfully +++
	@$(DIFF) $(LOGFILES).loggood $(LOGFILES).log
	@$(ECHO) +++ $(LOGFILES) passed successfully +++

install:

