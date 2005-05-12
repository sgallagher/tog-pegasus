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
ROOT = $(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/MCCATestClient
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
   FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
   SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

LIBRARIES = \
    pegmanagedclient \
    pegclient \
    pegcommon

PROGRAM = MCCATestClient
SOURCES = MCCATestClient.cpp

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifneq ($(OS),HPUX)
    ifdef PEGASUS_ENABLE_SLP
       LIBRARIES += pegslp_client
    endif
  endif
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests: repository
	
	$(PROGRAM) 1
	$(MAKE) -i unregister

# Load the sample MOF with the local compiler. Cleans the existing repository
# before reloading.
repository:

# Loading Core mof into the two new test namespaces	
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into root/test/A namespace ...
	@ cimmof "-I$(CIM_SCHEMA_DIR)" "-n/root/test/A" $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into root/test/B namespace ...
	@ cimmof  "-I$(CIM_SCHEMA_DIR)" "-n/root/test/B" $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof

# Loading MCCA_TestClass into the two namespaces
	@ $(ECHO) +++++ Loading MCCA_TestClass into root/test/A namespace ...
	@ cimmof  "-nroot/test/A" MCCA_TestClass.mof
	@ $(ECHO) +++++ Loading MCCA_TestClass into root/test/B namespace ...
	@ cimmof  "-nroot/test/B" MCCA_TestClass.mof
# Loading Association Schema definition into both namespaces		
	@ $(ECHO) +++++ Loading MCCA_TestAssocClass into root/test/A namespace ...
	@ cimmof  "-nroot/test/A" $(ROOT)/src/Providers/TestProviders/MCCA_TestAssocProvider/MCCA_TestAssocPrvSchema.mof
	@ $(ECHO) +++++ Loading MCCA_TestAssocClass into root/test/B namespace ...
	@ cimmof  "-nroot/test/B" $(ROOT)/src/Providers/TestProviders/MCCA_TestAssocProvider/MCCA_TestAssocPrvSchema.mof
# Registering Provider in PG_InterOp
	@ $(ECHO) +++++ Loading  MCCA_TestAssocClass provider definitions into PG_InterOp namespace ...
	@ cimmof  "-n$(INTEROPNS)" $(ROOT)/src/Providers/TestProviders/MCCA_TestAssocProvider/MCCA_TestAssocPrvReg.mof

removeclasses:
# Removing test repository without cimserver knowing it	
	@ $(ECHO) +++++ Removing MCCA_Test Classes from repository ...
	wbemexec DeleteClassA.xml > removeclasses.log
	wbemexec DeleteClassB.xml >> removeclasses.log
	wbemexec DeleteAssocA.xml >> removeclasses.log
	wbemexec DeleteAssocB.xml >> removeclasses.log

removerepository:
# Removing test repository without cimserver knowing it	
	@ $(ECHO) +++++ Removing existing repository named root/test/A
	@$(RMREPOSITORY) "$(REPOSITORY_ROOT)/root#test#A"
	@ $(ECHO) +++++ Removing existing repository named root/test/B
	@$(RMREPOSITORY) "$(REPOSITORY_ROOT)/root#test#B"

unregister: removeclasses
	@ $(ECHO) +++++ Unregister MCCA_TestAssocClass provider ...
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m MCCATestAssocProviderModule	
