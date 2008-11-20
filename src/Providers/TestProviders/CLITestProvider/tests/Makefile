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
# Test the cimcli test Provider using the script
#//==============================================================================
#//
#///////////////////////////////////////////////////////////////////////////////

ROOT = $(PEGASUS_ROOT)
DIR = Providers/TestProviders/CLITestProvider/tests

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

RESULTFILE = $(ROOT)/src/$(DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master

# sampleprovider\Load puts the sample MOF into a separate namespace.
# The following variable defines this namespace. Note that
# originally this Makefile was to do the install but that
# was moved to sampleprovider/load

PROVIDERNS=test/TestProvider

depend:

sub:

misc:

tests:

messages:

general:

poststarttests:

	@$(RM) $(RESULTFILE)
	@$(ECHO) 1. +++++ Test For Reference params

	@cimcli im Test_CLITestProviderClass  ReferenceParamTest -n "$(PROVIDERNS)" -ip a=[class.k1=v1,k2=v2,k3=v3]

	@$(ECHO) 2. +++++ Test String  params  which include square brackets
	@cimcli im Test_CLITestProviderClass  ReferenceParamTest -n "$(PROVIDERNS)" -ip a="\[string-param\]"

	@$(ECHO) 3. +++++ Test_CLITestProviderClass Instances Getproperty and setProperty for cimcli operation

#	@cimcli sp  -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" Name John
	@cimcli gp  -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" Name >> $(RESULTFILE)
	
	@$(DIFF) $(MASTERRESULTFILE) $(RESULTFILE)
	@$(ECHO) +++++ TST_Person Instances GetProperty and SetProperty tests Passed.

clean:

# END_OF_FILE
