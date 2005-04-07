ROOT = ../../../../../../

include $(ROOT)/mak/config.mak

LIBRARIES = \
     pegserver \
     pegclient \
     pegrepository \
     pegconfig \
     pegcommon

LOGFILES = TestCMPIInstanceExecQuery

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

SOURCES = TestCMPIInstanceExecQuery.cpp

include $(ROOT)/mak/program.mak
include $(ROOT)/test/config.mak


PROGRAM = TestCMPIInstanceExecQuery

LOGFILES = TestCMPIInstance

tests:

poststarttests: 
	@$(RM) $(LOGFILES).log
	@$(ECHO) $(PROGRAM) test
	@$(PROGRAM) test
	@$(ECHO) +++ $(PROGRAM) passed successfully +++
	@$(DIFF) $(LOGFILES).loggood $(LOGFILES).log
	@$(ECHO) +++ $(LOGFILES) passed successfully +++


install:
