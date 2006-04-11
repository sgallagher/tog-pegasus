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
ROOT = ../../..

DIR = /tests

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(TMP_DIR)/result
MASTERRESULTFILE = $(ROOT)/test/StressTests/$(DIR)/result.master

tests:

clean:
	@$(RM) $(RESULTFILE)

teststresstests:
	@$(RM) $(RESULTFILE)
	@$(ECHO) Running Usage tests:  
	@$(MAKE) -i -s stresstests_ignoreerror
	@$(STRIPCRS)
	$(COMPARERESULTS)
	@$(ECHO) +++ Usage Tests passed +++

stresstests:
	@$(ECHO) Starting cimserver
	@cimserver
	@$(ECHO) Running controller tests:  
	@stressTestController 
	@$(ECHO) +++ stressTestsController Tests completed +++
	@$(ECHO) Stoping cimserver:  
	@cimserver -s

stresstests_ignoreerror:
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -a >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -b >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -d >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -e >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -f >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -g >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -i >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -j >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -k >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -l >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -m >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -n >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -o >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -q >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -t >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -v >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -x >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -y >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@stressTestController -z >> $(RESULTFILE) $(REDIRECTERROR)

	@$(ECHO) "Missing required value for hostname - error" >> $(RESULTFILE)
	@stressTestController -h >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Missing required value for port - error" >> $(RESULTFILE)
	@stressTestController -p >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Missing required value for username - error" >> $(RESULTFILE)
	@stressTestController -u >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Missing required value for password - error" >> $(RESULTFILE)
	@stressTestController -w >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage" >> $(RESULTFILE)
	@stressTestController --help >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Missing configuration file - error" >> $(RESULTFILE)
	@stressTestController nofile.conf >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Unexpected Argument - error" >> $(RESULTFILE)
	@stressTestController nofile.conf anotherfile.conf>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Invalid Tolerance, Port number, & Host name  - error" >> $(RESULTFILE)
	@stressTestController invalid_configuration.conf >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "No clients specified in Configuration - error" >> $(RESULTFILE)
	@stressTestController invalid_noclients.conf>> $(RESULTFILE) $(REDIRECTERROR)
