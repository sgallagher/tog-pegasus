ROOT = ../../../../..

DIR = Pegasus/Compiler/cmdline/tests

MOF_PATH = $(ROOT)/Schemas/CIM25

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(ROOT)/src/$(DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master
TMPFILE = $(ROOT)/src/$(DIR)/tmp

NAMESPACE = -ntest

StrippedDiff:
	$(STRIPCRS)
	$(COMPARERESULTS)

NewTests:
	@$(RMDIRHIER) ./repository
	@cimmofl $(NAMESPACE) -R. -I$(MOF_PATH) $(MOF_PATH)/CIM_Core25.mof
	@$(ECHO) "cimmofl -R. ValidMof4.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof4.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/instances/ValidMof4.instances RESULTFILE=ValidMof4.instances.master
	@$(ECHO) "cimmofl -R. ValidMof5.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof5.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/instances/ValidMof5.instances RESULTFILE=ValidMof5.instances.master
	@$(ECHO) "cimmofl -R. ValidMof6.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof6.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/instances/ValidMof6.instances RESULTFILE=ValidMof6.instances.master
	@$(ECHO) "cimmofl -R. ValidMof7.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof7.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/A.B RESULTFILE=ValidMof7.A.master
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/B.# RESULTFILE=ValidMof7.B.master
	@$(ECHO) "cimmofl -R. ValidMof8.mof"
	@cimmofl $(NAMESPACE) -R. ValidMof8.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/ValidMof8a.# RESULTFILE=ValidMof8a.master
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/ValidMof8b.# RESULTFILE=ValidMof8b.master
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/ValidMof8c.# RESULTFILE=ValidMof8c.master
	@$(ECHO) "+++ Test Passed Successfully +++"

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
	@$(MAKE) -s NewTests
	@$(MAKE) -i -s defects

tests_ignoreerror:
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -h" >> $(RESULTFILE)
	@cimmofl -h >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl --help" >> $(RESULTFILE)
	@cimmofl --help >> $(RESULTFILE) $(REDIRECTERROR)
