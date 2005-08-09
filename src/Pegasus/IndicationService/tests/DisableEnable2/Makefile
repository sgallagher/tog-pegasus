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
DIR = Pegasus/IndicationService/tests/DisableEnable2
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY 

PROGRAM = TestDisableEnable2

SOURCES = DisableEnable2.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestDisableEnable2 setup WQL"
	@$(PROGRAM) setup WQL
	@$(ECHO) "TestDisableEnable2 sendFail WQL"
	@$(PROGRAM) sendFail WQL
	@$(ECHO) "TestDisableEnable2 create WQL"
	@$(PROGRAM) create WQL
	@$(ECHO) "TestDisableEnable2 sendSucceed WQL"
	@$(PROGRAM) sendSucceed WQL
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock WQL"
	@$(PROGRAM) sendBlock WQL
	@$(ECHO) "TestDisableEnable2 delete WQL"
	@$(PROGRAM) delete WQL
	@$(ECHO) "TestDisableEnable2 sendBlock WQL"
	@$(PROGRAM) sendBlock WQL
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendFail WQL"
	@$(PROGRAM) sendFail WQL
	@$(ECHO) "TestDisableEnable2 create WQL"
	@$(PROGRAM) create WQL
	@$(ECHO) "TestDisableEnable2 sendSucceed WQL"
	@$(PROGRAM) sendSucceed WQL
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock WQL"
	@$(PROGRAM) sendBlock WQL
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendSucceed WQL"
	@$(PROGRAM) sendSucceed WQL
	@$(ECHO) "TestDisableEnable2 delete WQL"
	@$(PROGRAM) delete WQL
	@$(ECHO) "TestDisableEnable2 sendFail WQL"
	@$(PROGRAM) sendFail WQL
	@$(ECHO) "TestDisableEnable2 cleanup WQL"
	@$(PROGRAM) cleanup WQL
ifndef PEGASUS_DISABLE_CQL
	@$(ECHO) "TestDisableEnable2 setup CIM:CQL"
	@$(PROGRAM) setup CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendFail CIM:CQL"
	@$(PROGRAM) sendFail CIM:CQL
	@$(ECHO) "TestDisableEnable2 create CIM:CQL"
	@$(PROGRAM) create CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendSucceed CIM:CQL"
	@$(PROGRAM) sendSucceed CIM:CQL
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock CIM:CQL"
	@$(PROGRAM) sendBlock CIM:CQL
	@$(ECHO) "TestDisableEnable2 delete CIM:CQL"
	@$(PROGRAM) delete CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendBlock CIM:CQL"
	@$(PROGRAM) sendBlock CIM:CQL
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendFail CIM:CQL"
	@$(PROGRAM) sendFail CIM:CQL
	@$(ECHO) "TestDisableEnable2 create CIM:CQL"
	@$(PROGRAM) create CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendSucceed CIM:CQL"
	@$(PROGRAM) sendSucceed CIM:CQL
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock CIM:CQL"
	@$(PROGRAM) sendBlock CIM:CQL
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendSucceed CIM:CQL"
	@$(PROGRAM) sendSucceed CIM:CQL
	@$(ECHO) "TestDisableEnable2 delete CIM:CQL"
	@$(PROGRAM) delete CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendFail CIM:CQL"
	@$(PROGRAM) sendFail CIM:CQL
	@$(ECHO) "TestDisableEnable2 cleanup CIM:CQL"
	@$(PROGRAM) cleanup CIM:CQL
endif
	@$(ECHO) "TestDisableEnable2 setup2 WQL"
	@$(PROGRAM) setup2 WQL
	@$(ECHO) "TestDisableEnable2 sendFail WQL"
	@$(PROGRAM) sendFail WQL
	@$(ECHO) "TestDisableEnable2 create2 WQL"
	@$(PROGRAM) create2 WQL
	@$(ECHO) "TestDisableEnable2 sendSucceed WQL"
	@$(PROGRAM) sendSucceed WQL
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock WQL"
	@$(PROGRAM) sendBlock WQL
	@$(ECHO) "TestDisableEnable2 delete2 WQL"
	@$(PROGRAM) delete2 WQL
	@$(ECHO) "TestDisableEnable2 sendBlock WQL"
	@$(PROGRAM) sendBlock WQL
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendFail WQL"
	@$(PROGRAM) sendFail WQL
	@$(ECHO) "TestDisableEnable2 create2 WQL"
	@$(PROGRAM) create2 WQL
	@$(ECHO) "TestDisableEnable2 sendSucceed WQL"
	@$(PROGRAM) sendSucceed WQL
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock WQL"
	@$(PROGRAM) sendBlock WQL
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendSucceed WQL"
	@$(PROGRAM) sendSucceed WQL
	@$(ECHO) "TestDisableEnable2 delete2 WQL"
	@$(PROGRAM) delete2 WQL
	@$(ECHO) "TestDisableEnable2 sendFail WQL"
	@$(PROGRAM) sendFail WQL
	@$(ECHO) "TestDisableEnable2 cleanup2 WQL"
	@$(PROGRAM) cleanup2 WQL
ifndef PEGASUS_DISABLE_CQL
	@$(ECHO) "TestDisableEnable2 setup2 CIM:CQL"
	@$(PROGRAM) setup2 CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendFail CIM:CQL"
	@$(PROGRAM) sendFail CIM:CQL
	@$(ECHO) "TestDisableEnable2 create2 CIM:CQL"
	@$(PROGRAM) create2 CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendSucceed CIM:CQL"
	@$(PROGRAM) sendSucceed CIM:CQL
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock CIM:CQL"
	@$(PROGRAM) sendBlock CIM:CQL
	@$(ECHO) "TestDisableEnable2 delete2 CIM:CQL"
	@$(PROGRAM) delete2 CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendBlock CIM:CQL"
	@$(PROGRAM) sendBlock CIM:CQL
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendFail CIM:CQL"
	@$(PROGRAM) sendFail CIM:CQL
	@$(ECHO) "TestDisableEnable2 create2 CIM:CQL"
	@$(PROGRAM) create2 CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendSucceed CIM:CQL"
	@$(PROGRAM) sendSucceed CIM:CQL
	cimprovider -d -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendBlock CIM:CQL"
	@$(PROGRAM) sendBlock CIM:CQL
	cimprovider -e -m RT_IndicationProviderModule
	@$(ECHO) "TestDisableEnable2 sendSucceed CIM:CQL"
	@$(PROGRAM) sendSucceed CIM:CQL
	@$(ECHO) "TestDisableEnable2 delete2 CIM:CQL"
	@$(PROGRAM) delete2 CIM:CQL
	@$(ECHO) "TestDisableEnable2 sendFail CIM:CQL"
	@$(PROGRAM) sendFail CIM:CQL
	@$(ECHO) "TestDisableEnable2 cleanup2 CIM:CQL"
	@$(PROGRAM) cleanup2 CIM:CQL
endif
	@$(ECHO) "+++++ TestDisableEnable2 passed all tests"
