#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================

ROOT = ../../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

# Steps to add a testcase:
# 1) Add your .query and .query.resgood file to this directory
# 2) Add a rule to build your .query.res file from a .query file 
# 3) Add the .query.res file to the tests target
# 4) Add the .query.res file to the clean target  

Like.query.res: Like.query
	@ cqlcli Like.query -test 1 -className CIM_ComputerSystem > $(TMP_DIR)/Like.query.res
	@ $(DIFF) Like.query.resgood $(TMP_DIR)/Like.query.res
	@ $(RM) $(TMP_DIR)/Like.query.res
	@ $(ECHO) +++ Like.query passed successfully +++

isa.query.res: isa.query
	@ cqlcli isa.query -test 1 -className CIM_ComputerSystem > $(TMP_DIR)/isa.query.res
	@ $(DIFF) isa.query.resgood $(TMP_DIR)/isa.query.res
	@ $(RM) $(TMP_DIR)/isa.query.res
	@ $(ECHO) +++ isa.query passed successfully +++

EmbeddedEvaluate.query.res: EmbeddedEvaluate.query
	@ cqlcli EmbeddedEvaluate.query -test 1 -className CQL_EmbeddedSubClass > $(TMP_DIR)/EmbeddedEvaluate.query.res
	@ $(DIFF) EmbeddedEvaluate.query.resgood $(TMP_DIR)/EmbeddedEvaluate.query.res
	@ $(RM) $(TMP_DIR)/EmbeddedEvaluate.query.res
	@ $(ECHO) +++ EmbeddedEvaluate.query passed successfully +++

SymbolicConstant.query.res: SymbolicConstant.query
	@ cqlcli SymbolicConstant.query -test 1 -className CIM_ComputerSystem > $(TMP_DIR)/SymbolicConstant.query.res
	@ $(DIFF) SymbolicConstant.query.resgood $(TMP_DIR)/SymbolicConstant.query.res
	@ $(RM) $(TMP_DIR)/SymbolicConstant.query.res
	@ $(ECHO) +++ SymbolicConstant.query passed successfully +++

ValidateProperties.query.res: ValidateProperties.query
	@ cqlcli ValidateProperties.query -test 4 -className CQL_TestPropertyTypes > $(TMP_DIR)/ValidateProperties.query.res
	@ $(DIFF) ValidateProperties.query.resgood $(TMP_DIR)/ValidateProperties.query.res
	@ $(RM) $(TMP_DIR)/ValidateProperties.query.res
	@ $(ECHO) +++ ValidateProperties.query passed successfully +++

GetPropertyList.query.res: GetPropertyList.query
	@ cqlcli GetPropertyList.query -test 3 -className CQL_TestElement > $(TMP_DIR)/GetPropertyList.query.res
	@ $(DIFF) GetPropertyList.query.resgood $(TMP_DIR)/GetPropertyList.query.res
	@ $(RM) $(TMP_DIR)/GetPropertyList.query.res
	@ $(ECHO) +++ GetPropertyList.query passed successfully +++

ApplyProjection.query.res: ApplyProjection.query
	@ cqlcli ApplyProjection.query -test 2 -className CQL_TestElement > $(TMP_DIR)/ApplyProjection.query.res
	@ $(DIFF) ApplyProjection.query.resgood $(TMP_DIR)/ApplyProjection.query.res
	@ $(RM) $(TMP_DIR)/ApplyProjection.query.res
	@ $(ECHO) +++ ApplyProjection.query passed successfully +++

EmbeddedSelect.query.res: EmbeddedSelect.query
	@ cqlcli EmbeddedSelect.query -test 2 -className CQL_EmbeddedSubClass > $(TMP_DIR)/EmbeddedSelect.query.res
	@ $(DIFF) EmbeddedSelect.query.resgood $(TMP_DIR)/EmbeddedSelect.query.res
	@ $(RM) $(TMP_DIR)/EmbeddedSelect.query.res
	@ $(ECHO) +++ EmbeddedSelect.query passed successfully +++

Misc.query.res: Misc.query
	@ cqlcli Misc.query -test 1 > $(TMP_DIR)/Misc.query.res
	@ $(DIFF) Misc.query.resgood $(TMP_DIR)/Misc.query.res
	@ $(RM) $(TMP_DIR)/Misc.query.res
	@ $(ECHO) +++ Misc.query passed successfully +++

NormalizeToDOC.query.res: NormalizeToDOC.query
	@ cqlcli NormalizeToDOC.query -test 5 -className CQL_TestPropertyTypes > $(TMP_DIR)/NormalizeToDOC.query.res
	@ $(DIFF) NormalizeToDOC.query.resgood $(TMP_DIR)/NormalizeToDOC.query.res
	@ $(RM) $(TMP_DIR)/NormalizeToDOC.query.res
	@ $(ECHO) +++ NormalizeToDOC.query passed successfully +++

general.query.res: general.query
	@ cqlcli general.query -test 1 -className CQL_TestPropertyTypes > $(TMP_DIR)/general.query.res
	@ $(DIFF) general.query.resgood $(TMP_DIR)/general.query.res
	@ $(RM) $(TMP_DIR)/general.query.res
	@ $(ECHO) +++ general.query passed successfully +++

Function.query.res: Function.query
	@ cqlcli Function.query -test 1 -className CQL_TestPropertyTypes > $(TMP_DIR)/Function.query.res
	@ $(DIFF) Function.query.resgood $(TMP_DIR)/Function.query.res
	@ $(RM) $(TMP_DIR)/Function.query.res
	@ $(ECHO) +++ Function.query passed successfully +++

Function2.query.res: Function2.query
	@ cqlcli Function2.query -test 1 -className CIM_RunningOS > $(TMP_DIR)/Function2.query.res
	@ $(DIFF) Function2.query.resgood $(TMP_DIR)/Function2.query.res
	@ $(RM) $(TMP_DIR)/Function2.query.res
	@ $(ECHO) +++ Function2.query passed successfully +++

clean:
	$(RM) Like.query.res
	$(RM) isa.query.res
	$(RM) SymbolicConstant.query.res
	$(RM) EmbeddedEvaluate.query.res
	$(RM) ValidateProperties.query.res
	$(RM) GetPropertyList.query.res
	$(RM) ApplyProjection.query.res
	$(RM) EmbeddedSelect.query.res
	$(RM) Misc.query.res
	$(RM) NormalizeToDOC.query.res
	$(RM) general.query.res
	$(RM) Function.query.res
	$(RM) Function2.query.res

depend:

tests:

poststarttests: \
	Like.query.res \
	isa.query.res \
	SymbolicConstant.query.res \
	EmbeddedEvaluate.query.res \
	ValidateProperties.query.res \
	GetPropertyList.query.res \
	ApplyProjection.query.res \
	EmbeddedSelect.query.res \
	Misc.query.res \
	NormalizeToDOC.query.res \
	general.query.res \
	Function.query.res \
	Function2.query.res


general:

