#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
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
# Test the new CMPI performance test providers for functionality

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
SHH = @

DIR = Providers/TestProviders/PerformanceTests/CMPI/tests

PROVIDERNS=test/TestProvider

clean:

poststarttests:
	@$(ECHO) 1. +++++ Test all enumerate operations on the classes

	$(SHH)cimcli ei "cmpiPerf_TestClassA" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)cimcli ni "cmpiPerf_TestClassA" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)cimcli ei "cmpiPerf_TestClassB" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)cimcli ni "cmpiPerf_TestClassB" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)cimcli ei "cmpiPerf_TestAssocClass" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)cimcli ni "cmpiPerf_TestAssocClass" -n "$(PROVIDERNS)" --sum -count 1

	@$(ECHO) 2. +++++ Testing getInstance CMPI Perftest providers in $(PROVIDERNS)

	$(SHH)cimcli gi "cmpiPerf_TestClassA.theKey=2" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)cimcli gi "cmpiPerf_TestClassB.theKey=0" -n "$(PROVIDERNS)" --sum -count 1

	@$(ECHO) 3. +++++ Testing associate operations CMPI Perftest in $(PROVIDERNS)

	$(SHH)cimcli an "cmpiPerf_TestClassA.theKey=2" -n "$(PROVIDERNS)" --sum -count 1
	$(SHH)cimcli a  "cmpiPerf_TestClassA.theKey=2" -n "$(PROVIDERNS)" --sum -count 1

# END_OF_FILE
