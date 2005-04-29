ROOT = ../../../../../../
include $(ROOT)/mak/config.mak

LIBRARIES = \
     pegserver \
     pegclient \
     pegrepository \
     pegconfig \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

SOURCES = TestCMPIMethod.cpp

include $(ROOT)/mak/program.mak
include $(ROOT)/test/config.mak


PROGRAM = TestCMPIMethod

LOGFILES = TestCMPIMethod

tests:

poststarttests: 
	@$(RM) $(LOGFILES).log
	@$(ECHO) $(PROGRAM) test
	@$(PROGRAM) test
	@$(ECHO) +++ $(PROGRAM) passed successfully +++
	@$(DIFF) $(LOGFILES).loggood $(LOGFILES).log
	@$(ECHO) +++ $(LOGFILES) passed successfully +++


install:
