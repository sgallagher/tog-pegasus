
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
# 4) Add the .query.res file to the clean target  

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

ValidateProperties.query.res: ValidateProperties.query
	@ cqlcli ValidateProperties.query -test 4 -className CQL_TestPropertyTypes > $(TMP_DIR)/ValidateProperties.query.res | cd .
	@ $(DIFF) ValidateProperties.query.resgood $(TMP_DIR)/ValidateProperties.query.res
	@ $(RM) $(TMP_DIR)/ValidateProperties.query.res
	@ $(ECHO) +++ ValidateProperties.query passed successfully +++

GetPropertyList.query.res: GetPropertyList.query
	@ cqlcli GetPropertyList.query -test 3 -className CQL_TestElement > $(TMP_DIR)/GetPropertyList.query.res | cd .
	@ $(DIFF) GetPropertyList.query.resgood $(TMP_DIR)/GetPropertyList.query.res
	@ $(RM) $(TMP_DIR)/GetPropertyList.query.res
	@ $(ECHO) +++ GetPropertyList.query passed successfully +++

ApplyProjection.query.res: ApplyProjection.query
	@ cqlcli ApplyProjection.query -test 2 -className CQL_TestElement > $(TMP_DIR)/ApplyProjection.query.res | cd .
	@ $(DIFF) ApplyProjection.query.resgood $(TMP_DIR)/ApplyProjection.query.res
	@ $(RM) $(TMP_DIR)/ApplyProjection.query.res
	@ $(ECHO) +++ ApplyProjection.query passed successfully +++

EmbeddedSelect.query.res: EmbeddedSelect.query
	@ cqlcli EmbeddedSelect.query -test 2 -className CQL_EmbeddedSubClass > $(TMP_DIR)/EmbeddedSelect.query.res | cd .
	@ $(DIFF) EmbeddedSelect.query.resgood $(TMP_DIR)/EmbeddedSelect.query.res
	@ $(RM) $(TMP_DIR)/EmbeddedSelect.query.res
	@ $(ECHO) +++ EmbeddedSelect.query passed successfully +++

clean:
	$(RM) Like.query.res
	$(RM) isa.query.res
	$(RM) SymbolicConstant.query.res
	$(RM) EmbeddedEvaluate.query.res
	$(RM) ValidateProperties.query.res
	$(RM) GetPropertyList.query.res
	$(RM) ApplyProjection.query.res
	$(RM) EmbeddedSelect.query.res

depend:

tests: \
	Like.query.res \
	isa.query.res \
	SymbolicConstant.query.res \
	EmbeddedEvaluate.query.res \
	ValidateProperties.query.res \
	GetPropertyList.query.res \
	ApplyProjection.query.res \
	EmbeddedSelect.query.res  

poststarttests:

general:

install:
