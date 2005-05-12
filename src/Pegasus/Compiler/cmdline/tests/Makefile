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

DIR = Pegasus/Compiler/cmdline/tests

MOF_PATH = $(ROOT)/Schemas/CIM25

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(TMP_DIR)/cmdline_result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master
TMPFILE = $(TMP_DIR)/cmdline_tmp
REPOSITORYDIR = $(TMP_DIR)

NAMESPACE = -ntest

StrippedDiff:
	$(STRIPCRS)
	$(COMPARERESULTS)

NewTests:
	@$(RMDIRHIER) $(REPOSITORYDIR)/repository
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" "-I$(MOF_PATH)" $(MOF_PATH)/CIM_Core25.mof
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof4.mof"
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof4.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/instances/ValidMof4.instances RESULTFILE=ValidMof4.instances.master
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof4a.mof"
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof4a.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/instances/ValidMof4a.instances RESULTFILE=ValidMof4.instances.master
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof4b.mof"
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof4b.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/instances/ValidMof4b.instances RESULTFILE=ValidMof4.instances.master
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof5.mof"
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof5.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/instances/ValidMof5.instances RESULTFILE=ValidMof5.instances.master
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof6.mof"
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof6.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/instances/ValidMof6.instances RESULTFILE=ValidMof6.instances.master
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof7.mof"
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof7.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/A.B RESULTFILE=ValidMof7.A.master
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/B.# RESULTFILE=ValidMof7.B.master
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof8.mof"
	@cimmofl $(NAMESPACE) -R$(REPOSITORYDIR) ValidMof8.mof
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/ValidMof8a.# RESULTFILE=ValidMof8a.master
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/ValidMof8b.# RESULTFILE=ValidMof8b.master
	@$(MAKE) -s MASTERRESULTFILE=./repository/test/classes/ValidMof8c.# RESULTFILE=ValidMof8c.master
	@$(ECHO) "+++ Test Passed +++"

clean:
	@$(RM) $(TMPFILE)
	@$(RM) $(RESULTFILE)
	@$(RMDIRHIER) $(REPOSITORYDIR)/repository

hangdefects:
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl with no arguments hangs" >> output
	@cimmofl >> $(RESULTFILE) $(REDIRECTERROR)

defects:
	@$(ECHO) "cimmofl -E InvalidMof1.mof - InvalidQualifier" >> $(RESULTFILE)
	@cimmofl "-E" InvalidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -E ValidMof2.mof" >> $(RESULTFILE)
	@cimmofl "-E" ValidMof2.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -xml ValidMof2.mof" >> $(RESULTFILE)
	@cimmofl --xml ValidMof1.mof >> $(TMPFILE) $(REDIRECTERROR)

tests:
	@$(RM) $(RESULTFILE)
	@$(RM) $(TMPFILE)
	@$(RMDIRHIER) $(REPOSITORYDIR)/repository
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" "-I$(MOF_PATH)" $(MOF_PATH)/CIM_Core25.mof
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -xml ValidMof1.mof" >> $(RESULTFILE)
	@cimmofl --xml ValidMof1.mof >> $(TMPFILE) 
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -E ValidMof1.mof" >> $(RESULTFILE)
	@cimmofl "-E" ValidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof1.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof1.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl $(NAMESPACE) -R$(REPOSITORYDIR) -xml ValidMof2.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) -R$(REPOSITORYDIR) --xml ValidMof2.mof >> $(TMPFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl $(NAMESPACE) -R$(REPOSITORYDIR) ValidMof2.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof2.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof3.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof3.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof4.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof4.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof4a.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof4a.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof4b.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof4b.mof >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "cimmofl -R$(REPOSITORYDIR) ValidMof5.mof" >> $(RESULTFILE)
	@cimmofl $(NAMESPACE) "-R$(REPOSITORYDIR)" ValidMof5.mof >> $(RESULTFILE) $(REDIRECTERROR)
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

depend:

poststarttests:

