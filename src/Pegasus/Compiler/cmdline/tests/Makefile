ROOT = ../../../../..

DIR = Pegasus/Compiler/cmdline/tests

MOF_PATH = $(ROOT)/Schemas/CIM25

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(ROOT)/src/$(DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master
TMPFILE = $(ROOT)/src/$(DIR)/tmp

NAMESPACE = -ntest

NewTests:
	@$(RMDIRHIER) ./repository
	@cimmofl $(NAMESPACE) -R. -I$(MOF_PATH) $(MOF_PATH)/CIM_Core25.mof
	@$(ECHO) "cimmofl -R. ValidMof4.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof4.mof
	@$(DIFF) ./repository/test/instances/ValidMof4.instances ValidMof4.instances.master
	@$(ECHO) "*** Test Passed Successfully ***"
	@$(ECHO) "cimmofl -R. ValidMof5.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof5.mof
	@$(DIFF) ./repository/test/instances/ValidMof5.instances ValidMof5.instances.master
	@$(ECHO) "*** Test Passed Successfully ***"
	@$(ECHO) "cimmofl -R. ValidMof6.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof6.mof
	@$(DIFF) ./repository/test/instances/ValidMof6.instances ValidMof6.instances.master
	@$(ECHO) "*** Test Passed Successfully ***"
	@$(ECHO) "cimmofl -R. ValidMof7.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof7.mof
	@$(DIFF) ./repository/test/classes/A.B ValidMof7.A.master
	@$(DIFF) ./repository/test/classes/B.# ValidMof7.B.master
	@$(ECHO) "*** Test Passed Successfully ***"

clean:
	@$(RM) $(TMPFILE)
	@$(RM) $(RESULTFILE)
	@$(RMDIRHIER) ./repository

install:

hangdefects:
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl with no arguments hangs" >> output
	@cimmofl >> $(RESULTFILE) $(REDIRECTERROR)

defects:
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R. Char16.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) -R. Char16.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -E InvalidMof1.mof - InvalidQualifier" >> $(RESULTFILE)
	@cimmofl -E InvalidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -E ValidMof2.mof" >> $(RESULTFILE)
	@cimmofl -E ValidMof2.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -xml ValidMof2.mof" >> $(RESULTFILE)
	@cimmofl --xml ValidMof1.mof >> $(TMPFILE) $(REDIRECTERROR)

tests:
	@$(RM) $(RESULTFILE)
	@$(RM) $(TMPFILE)
	@$(RMDIRHIER) ./repository
	@cimmofl $(NAMESPACE) -R. -I$(MOF_PATH) $(MOF_PATH)/CIM_Core25.mof
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -xml ValidMof1.mof" >> $(RESULTFILE)
	@cimmofl --xml ValidMof1.mof >> $(TMPFILE) 
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -E ValidMof1.mof" >> $(RESULTFILE)
	@cimmofl -E ValidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R. ValidMof1.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) -R. ValidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -xml ValidMof2.mof" >> $(RESULTFILE)
	@cimmofl --xml ValidMof2.mof >> $(TMPFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R. ValidMof2.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) -R. ValidMof2.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R. ValidMof3.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) -R. ValidMof3.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R. ValidMof4.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) -R. ValidMof4.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R. ValidMof5.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) -R. ValidMof5.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(MAKE) -i -s tests_ignoreerror
	$(COMPARERESULTS)
	@$(ECHO) +++ Test Passed +++
	@$(MAKE) -i -s defects


tests_ignoreerror:
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -h" >> $(RESULTFILE)
	@cimmofl -h >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl --help" >> $(RESULTFILE)
	@cimmofl --help >> $(RESULTFILE) $(REDIRECTERROR)
