ROOT = ../../../../..

DIR = Pegasus/Compiler/cmdline/tests

MOF_PATH = $(ROOT)/Schemas/CIM25

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(ROOT)/src/$(DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master
TMPFILE = $(ROOT)/src/$(DIR)/tmp

NAMESPACE = -ntest


clean:
	@$(RM) $(TMPFILE)
	@$(RM) $(RESULTFILE)
	@$(RMDIRHIER) ./repository

install:

hangdefects:
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof with no arguments hangs" >> output
	@cimmof >> $(RESULTFILE) $(REDIRECTERROR)

defects:
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -R. Char16.mof" >> $(RESULTFILE)
	@cimmof $(NAMESPACE) -R. Char16.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -E InvalidMof1.mof - InvalidQualifier" >> $(RESULTFILE)
	@cimmof -E InvalidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -E ValidMof2.mof" >> $(RESULTFILE)
	@cimmof -E ValidMof2.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -xml ValidMof2.mof" >> $(RESULTFILE)
	@cimmof --xml ValidMof1.mof >> $(TMPFILE) $(REDIRECTERROR)

tests:
	@$(RM) $(RESULTFILE)
	@$(RM) $(TMPFILE)
	@$(RMDIRHIER) ./repository
	@cimmof $(NAMESPACE) -R. -I$(MOF_PATH) $(MOF_PATH)/CIM_Core25.mof
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -xml ValidMof1.mof" >> $(RESULTFILE)
	@cimmof --xml ValidMof1.mof >> $(TMPFILE) 
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -E ValidMof1.mof" >> $(RESULTFILE)
	@cimmof -E ValidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -R. ValidMof1.mof" >> $(RESULTFILE)
	@cimmof $(NAMESPACE) -R. ValidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -xml ValidMof2.mof" >> $(RESULTFILE)
	@cimmof --xml ValidMof2.mof >> $(TMPFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -R. ValidMof2.mof" >> $(RESULTFILE)
	@cimmof $(NAMESPACE) -R. ValidMof2.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(MAKE) -i -s tests_ignoreerror
	$(COMPARERESULTS)
	@$(ECHO) +++ Test Passed +++
	@$(MAKE) -i -s defects

tests_ignoreerror:
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof -h" >> $(RESULTFILE)
	@cimmof -h >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmof --help" >> $(RESULTFILE)
	@cimmof --help >> $(RESULTFILE) $(REDIRECTERROR)
