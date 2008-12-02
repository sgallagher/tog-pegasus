#//%LICENSE////////////////////////////////////////////////////////////////
#//
#// Licensed to The Open Group (TOG) under one or more contributor license
#// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
#// this work for additional information regarding copyright ownership.
#// Each contributor licenses this file to you under the OpenPegasus Open
#// Source License; you may not use this file except in compliance with the
#// License.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a
#// copy of this software and associated documentation files (the "Software"),
#// to deal in the Software without restriction, including without limitation
#// the rights to use, copy, modify, merge, publish, distribute, sublicense,
#// and/or sell copies of the Software, and to permit persons to whom the
#// Software is furnished to do so, subject to the following conditions:
#//
#// The above copyright notice and this permission notice shall be included
#// in all copies or substantial portions of the Software.
#//
#// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//////////////////////////////////////////////////////////////////////////
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
TABLEFORMATFILE = $(ROOT)/src/$(DIR)/tableformat
MASTERTABLEFORMATFILE = $(ROOT)/src/$(DIR)/tableformat.master

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
	@$(RM) $(TABLEFORMATFILE)
	@$(ECHO) 1. +++++ Test For Reference params

	@cimcli im Test_CLITestProviderClass  ReferenceParamTest -n "$(PROVIDERNS)" -ip a=[class.k1=v1,k2=v2,k3=v3]

	@$(ECHO) 2. +++++ Test String  params  which include square brackets
	@cimcli im Test_CLITestProviderClass  ReferenceParamTest -n "$(PROVIDERNS)" -ip a="\[string-param\]"

	@$(ECHO) 3. +++++ Test_CLITestProviderClass Instances Getproperty and setProperty for cimcli operation

#	@cimcli sp  -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" Name John
	@cimcli gp  -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Mike\" Name >> $(RESULTFILE)
	
	@$(DIFF) $(MASTERRESULTFILE) $(RESULTFILE)
	@$(ECHO) +++++ TST_Person Instances GetProperty and SetProperty tests Passed.
	@$(ECHO)

	@$(ECHO) 4. +++++ Test table output format

	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=Norman Name=Susan
	@cimcli ci -n "$(PROVIDERNS)" Test_CLITestProviderClass Id=Thomas Name=Peter
	@cimcli ei -n "$(PROVIDERNS)" -o table Test_CLITestProviderClass >> $(TABLEFORMATFILE) 
	@$(DIFF) $(MASTERTABLEFORMATFILE) $(TABLEFORMATFILE)
	@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Norman\"
	@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Thomas\"
	@$(RM) $(RESULTFILE)
	@$(RM) $(TABLEFORMATFILE)
	@$(ECHO) +++++ Test table output format Passed.

clean:
	-@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Norman\"
	-@cimcli di -n "$(PROVIDERNS)" Test_CLITestProviderClass.Id=\"Thomas\"
	@$(RM) $(RESULTFILE)
	@$(RM) $(TABLEFORMATFILE)

# END_OF_FILE
