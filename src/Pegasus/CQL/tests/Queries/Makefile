
ROOT = ../../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
    DIFF = mu compare
else
    DIFF = diff
endif

# Steps to add a testcase:
# 1) Add your .query and .query.resgood file to this directory
# 2) Add a rule to build your .query.res file from a .query file 
# 3) Add the .query.res file to the tests target  BUT NOT YET!!!!!
# 4) Add the .query.res file to the clean target  BUT NOT YET!!!!!

Like.query.res: Like.query
	@ cqlcli Like.query -test 1 -className CQL_TestPropertyTypes > $(TMP_DIR)/Like.query.res | cd .
	@ $(DIFF) Like.query.resgood $(TMP_DIR)/Like.query.res
	@ $(RM) $(TMP_DIR)/Like.query.res
	@ $(ECHO) +++ Like.query passed successfully +++

isa.query.res: isa.query
	@ cqlcli isa.query -test 1 -className CIM_ComputerSystem > $(TMP_DIR)/isa.query.res | cd .
	@ $(DIFF) isa.query.resgood $(TMP_DIR)/isa.query.res
	@ $(RM) $(TMP_DIR)/isa.query.res
	@ $(ECHO) +++ isa.query passed successfully +++

EmbeddedEvaluate.query.res: EmbeddedEvaluate.query
	@ cqlcli EmbeddedEvaluate.query -test 1 -className CQL_EmbeddedSubClass > $(TMP_DIR)/EmbeddedEvaluate.query.res | cd .
	@ $(DIFF) EmbeddedEvaluate.query.resgood $(TMP_DIR)/EmbeddedEvaluate.query.res
	@ $(RM) $(TMP_DIR)/EmbeddedEvaluate.query.res
	@ $(ECHO) +++ EmbeddedEvaluate.query passed successfully +++

SymbolicConstant.query.res: SymbolicConstant.query
	@ cqlcli SymbolicConstant.query -test 1 -className CIM_ComputerSystem > $(TMP_DIR)/SymbolicConstant.query.res | cd .
	@ $(DIFF) SymbolicConstant.query.resgood $(TMP_DIR)/SymbolicConstant.query.res
	@ $(RM) $(TMP_DIR)/SymbolicConstant.query.res
	@ $(ECHO) +++ SymbolicConstant.query passed successfully +++

clean:
	$(RM) Like.query.res
	$(RM) isa.query.res
	$(RM) SymbolicConstant.query.res
	$(RM) EmbeddedEvaluate.query.res

depend:

tests:

poststarttests:

general:

install:
