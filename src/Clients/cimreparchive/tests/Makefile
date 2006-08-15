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
ROOT = ../../../..

DIR = Clients/cimreparchive/tests

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(TMP_DIR)/result
IGNOREFILE =  $(TMP_DIR)/ignore
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master

tests:

clean:
	@$(RM) $(RESULTFILE)

poststarttests:
	@$(RM) $(RESULTFILE)
	@ # Test error cases
	@$(MAKE) -i -s poststarttests_ignoreerror
	@ # Test success case (prints an environment-specific message)
	@$(ECHO) "Success" >> $(RESULTFILE)
	@cimreparchive rep.tar > $(IGNOREFILE)
	@$(RM) $(IGNOREFILE)
	@$(RM) rep.tar
	@ # Check the results
	@$(STRIPCRS)
	$(COMPARERESULTS)
	@$(ECHO) +++ Test passed +++
	@$(RM) $(RESULTFILE)

poststarttests_ignoreerror:
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimreparchive >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimreparchive -a >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimreparchive a b >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimreparchive --help a >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimreparchive --bogus >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate --help option" >> $(RESULTFILE)
	@cimreparchive --help --help >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate --version option" >> $(RESULTFILE)
	@cimreparchive --version --version >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Show help" >> $(RESULTFILE)
	@cimreparchive --help >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Show version" >> $(RESULTFILE)
	@cimreparchive --version >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Invalid archive file" >> $(RESULTFILE)
	# The message printed by this test case is platform-specific
	@cimreparchive a/b/c.tar > $(IGNOREFILE) $(REDIRECTERROR)
	@$(RM) $(IGNOREFILE)

depend:

general:

strip-license:

prepend-license:
