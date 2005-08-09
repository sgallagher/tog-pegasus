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
ROOT = ../../../..
DIR = Unsupported/LifecycleIndicationProviders/testclient
include $(ROOT)/mak/config.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes
LIBRARIES = \
     pegclient \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

PROGRAM = TestLifecycleIndication

SOURCES = LifecycleIndication.cpp

SAMPLEPROVIDERNS="root/SampleProvider"
SAMPLECLASS="Sample_LifecycleIndicationProviderClass"
	  
include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestLifecycleIndication setup WQL"
	@$(PROGRAM) setup WQL
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "How many instances exist? Lets enumerate them..."
	-CLI ei -n $(SAMPLEPROVIDERNS) $(SAMPLECLASS)
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "We've deleted two instances, should just be one left..."
	-CLI ei -n $(SAMPLEPROVIDERNS) $(SAMPLECLASS)
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "TestLifecycleIndication cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "TestLifecycleIndication setup CIM:CQL"
	@$(PROGRAM) setup CIM:CQL
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "How many instances exist? Lets enumerate them..."
	-CLI ei -n $(SAMPLEPROVIDERNS) $(SAMPLECLASS)
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "We've deleted two instances, should just be one left..."
	-CLI ei -n $(SAMPLEPROVIDERNS) $(SAMPLECLASS)
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "TestLifecycleIndication cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "+++++ TestLifecycleIndication passed all tests"
