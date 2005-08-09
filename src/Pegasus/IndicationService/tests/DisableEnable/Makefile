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
DIR = Pegasus/IndicationService/tests/DisableEnable
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = TestDisableEnable

SOURCES = DisableEnable.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestDisableEnable setup WQL"
	@$(PROGRAM) setup WQL
	@$(ECHO) "TestDisableEnable create WQL"
	@$(PROGRAM) create WQL
	cimprovider -d -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete WQL"
	@$(PROGRAM) delete WQL
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable create WQL"
	@$(PROGRAM) create WQL
	cimprovider -d -m ProcessIndicationProviderModule
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete WQL"
	@$(PROGRAM) delete WQL
	@$(ECHO) "TestDisableEnable create WQL"
	@$(PROGRAM) create WQL
	@$(ECHO) "TestDisableEnable create2 WQL"
	@$(PROGRAM) create2 WQL
	@$(ECHO) "TestDisableEnable addProvider WQL"
	@$(PROGRAM) addProvider WQL
	@$(ECHO) "TestDisableEnable modifyProvider1 WQL"
	@$(PROGRAM) modifyProvider1 WQL
	@$(ECHO) "TestDisableEnable modifyProvider2 WQL"
	@$(PROGRAM) modifyProvider2 WQL
	@$(ECHO) "TestDisableEnable deleteProvider WQL"
	@$(PROGRAM) deleteProvider WQL
	@$(ECHO) "TestDisableEnable delete2 WQL"
	@$(PROGRAM) delete2 WQL
	@$(ECHO) "TestDisableEnable delete WQL"
	@$(PROGRAM) delete WQL
	@$(ECHO) "TestDisableEnable cleanup WQL"
	@$(PROGRAM) cleanup WQL
ifndef PEGASUS_DISABLE_CQL
	@$(ECHO) "TestDisableEnable setup CIM:CQL"
	@$(PROGRAM) setup CIM:CQL
	@$(ECHO) "TestDisableEnable create CIM:CQL"
	@$(PROGRAM) create CIM:CQL
	cimprovider -d -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete CIM:CQL"
	@$(PROGRAM) delete CIM:CQL
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable create CIM:CQL"
	@$(PROGRAM) create CIM:CQL
	cimprovider -d -m ProcessIndicationProviderModule
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete CIM:CQL"
	@$(PROGRAM) delete CIM:CQL
	@$(ECHO) "TestDisableEnable create CIM:CQL"
	@$(PROGRAM) create CIM:CQL
	@$(ECHO) "TestDisableEnable create2 CIM:CQL"
	@$(PROGRAM) create2 CIM:CQL
	@$(ECHO) "TestDisableEnable addProvider CIM:CQL"
	@$(PROGRAM) addProvider CIM:CQL
	@$(ECHO) "TestDisableEnable modifyProvider1 CIM:CQL"
	@$(PROGRAM) modifyProvider1 CIM:CQL
	@$(ECHO) "TestDisableEnable modifyProvider2 CIM:CQL"
	@$(PROGRAM) modifyProvider2 CIM:CQL
	@$(ECHO) "TestDisableEnable deleteProvider CIM:CQL"
	@$(PROGRAM) deleteProvider CIM:CQL
	@$(ECHO) "TestDisableEnable delete2 CIM:CQL"
	@$(PROGRAM) delete2 CIM:CQL
	@$(ECHO) "TestDisableEnable delete CIM:CQL"
	@$(PROGRAM) delete CIM:CQL
	@$(ECHO) "TestDisableEnable cleanup CIM:CQL"
	@$(PROGRAM) cleanup CIM:CQL
	@$(ECHO) "+++++ TestDisableEnable passed all tests"
endif
