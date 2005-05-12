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
	@$(ECHO) "TestDisableEnable setup wql"
	@$(PROGRAM) setup wql
	@$(ECHO) "TestDisableEnable create wql"
	@$(PROGRAM) create wql
	cimprovider -d -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete wql"
	@$(PROGRAM) delete wql
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable create wql"
	@$(PROGRAM) create wql
	cimprovider -d -m ProcessIndicationProviderModule
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete wql"
	@$(PROGRAM) delete wql
	@$(ECHO) "TestDisableEnable create wql"
	@$(PROGRAM) create wql
	@$(ECHO) "TestDisableEnable create2 wql"
	@$(PROGRAM) create2 wql
	@$(ECHO) "TestDisableEnable addProvider wql"
	@$(PROGRAM) addProvider wql
	@$(ECHO) "TestDisableEnable modifyProvider1 wql"
	@$(PROGRAM) modifyProvider1 wql
	@$(ECHO) "TestDisableEnable modifyProvider2 wql"
	@$(PROGRAM) modifyProvider2 wql
	@$(ECHO) "TestDisableEnable deleteProvider wql"
	@$(PROGRAM) deleteProvider wql
	@$(ECHO) "TestDisableEnable delete2 wql"
	@$(PROGRAM) delete2 wql
	@$(ECHO) "TestDisableEnable delete wql"
	@$(PROGRAM) delete wql
	@$(ECHO) "TestDisableEnable cleanup wql"
	@$(PROGRAM) cleanup wql
ifndef PEGASUS_DISABLE_CQL
	@$(ECHO) "TestDisableEnable setup cim:cql"
	@$(PROGRAM) setup cim:cql
	@$(ECHO) "TestDisableEnable create cim:cql"
	@$(PROGRAM) create cim:cql
	cimprovider -d -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete cim:cql"
	@$(PROGRAM) delete cim:cql
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable create cim:cql"
	@$(PROGRAM) create cim:cql
	cimprovider -d -m ProcessIndicationProviderModule
	cimprovider -e -m ProcessIndicationProviderModule
	@$(ECHO) "TestDisableEnable delete cim:cql"
	@$(PROGRAM) delete cim:cql
	@$(ECHO) "TestDisableEnable create cim:cql"
	@$(PROGRAM) create cim:cql
	@$(ECHO) "TestDisableEnable create2 cim:cql"
	@$(PROGRAM) create2 cim:cql
	@$(ECHO) "TestDisableEnable addProvider cim:cql"
	@$(PROGRAM) addProvider cim:cql
	@$(ECHO) "TestDisableEnable modifyProvider1 cim:cql"
	@$(PROGRAM) modifyProvider1 cim:cql
	@$(ECHO) "TestDisableEnable modifyProvider2 cim:cql"
	@$(PROGRAM) modifyProvider2 cim:cql
	@$(ECHO) "TestDisableEnable deleteProvider cim:cql"
	@$(PROGRAM) deleteProvider cim:cql
	@$(ECHO) "TestDisableEnable delete2 cim:cql"
	@$(PROGRAM) delete2 cim:cql
	@$(ECHO) "TestDisableEnable delete cim:cql"
	@$(PROGRAM) delete cim:cql
	@$(ECHO) "TestDisableEnable cleanup cim:cql"
	@$(PROGRAM) cleanup cim:cql
	@$(ECHO) "+++++ TestDisableEnable passed all tests"
endif
