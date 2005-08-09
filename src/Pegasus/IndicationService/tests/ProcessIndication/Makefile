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
DIR = Pegasus/IndicationService/tests/ProcessIndication
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

PROGRAM = TestProcessIndication

SOURCES = ProcessIndication.cpp

ifeq ($(OS),VMS)
LOCAL_DEFINES = -DINDICATION_DIR="""$(DISPLAYCONSUMER_DIR)""" -DPEGASUS_INTERNALONLY
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestProcessIndication setup WQL"
	@$(PROGRAM) setup WQL
	@$(ECHO) "TestProcessIndication create1 WQL"
	@$(PROGRAM) create1 WQL
	@$(ECHO) "TestProcessIndication sendNormal WQL"
	@$(PROGRAM) sendNormal WQL
	@$(ECHO) "TestProcessIndication checkNormal WQL"
	@$(PROGRAM) checkNormal WQL
	@$(ECHO) "TestProcessIndication sendSubclass WQL"
	@$(PROGRAM) sendSubclass WQL
	@$(ECHO) "TestProcessIndication checkSubclass WQL"
	@$(PROGRAM) checkSubclass WQL
	@$(ECHO) "TestProcessIndication sendMissing WQL"
	@$(PROGRAM) sendMissing WQL
	@$(ECHO) "TestProcessIndication checkMissing WQL"
	@$(PROGRAM) checkMissing WQL
	@$(ECHO) "TestProcessIndication sendExtra WQL"
	@$(PROGRAM) sendExtra WQL
	@$(ECHO) "TestProcessIndication checkExtra WQL"
	@$(PROGRAM) checkExtra WQL
	@$(ECHO) "TestProcessIndication sendMatching WQL"
	@$(PROGRAM) sendMatching WQL
	@$(ECHO) "TestProcessIndication checkMatching WQL"
	@$(PROGRAM) checkMatching WQL
	@$(ECHO) "TestProcessIndication sendUnmatchingNamespace WQL"
	@$(PROGRAM) sendUnmatchingNamespace WQL
	@$(ECHO) "TestProcessIndication checkUnmatchingNamespace WQL"
	@$(PROGRAM) checkUnmatchingNamespace WQL
	@$(ECHO) "TestProcessIndication sendUnmatchingClassName WQL"
	@$(PROGRAM) sendUnmatchingClassName WQL
	@$(ECHO) "TestProcessIndication checkUnmatchingClassName WQL"
	@$(PROGRAM) checkUnmatchingClassName WQL
	@$(ECHO) "TestProcessIndication delete1 WQL"
	@$(PROGRAM) delete1 WQL
	@$(ECHO) "TestProcessIndication create2 WQL"
	@$(PROGRAM) create2 WQL
	@$(ECHO) "TestProcessIndication sendNormal WQL"
	@$(PROGRAM) sendNormal WQL
	@$(ECHO) "TestProcessIndication checkNormalAll WQL"
	@$(PROGRAM) checkNormalAll WQL
	@$(ECHO) "TestProcessIndication sendMissing WQL"
	@$(PROGRAM) sendMissing WQL
	@$(ECHO) "TestProcessIndication checkMissingAll WQL"
	@$(PROGRAM) checkMissingAll WQL
	@$(ECHO) "TestProcessIndication sendExtra WQL"
	@$(PROGRAM) sendExtra WQL
	@$(ECHO) "TestProcessIndication checkExtraAll WQL"
	@$(PROGRAM) checkExtraAll WQL
	@$(ECHO) "TestProcessIndication delete2 WQL"
	@$(PROGRAM) delete2 WQL
	@$(ECHO) "TestProcessIndication cleanup WQL"
	@$(PROGRAM) cleanup WQL
	@$(ECHO) "TestProcessIndication setup CIM:CQL"
	@$(PROGRAM) setup CIM:CQL
	@$(ECHO) "TestProcessIndication create1 CIM:CQL"
	@$(PROGRAM) create1 CIM:CQL
	@$(ECHO) "TestProcessIndication sendNormal CIM:CQL"
	@$(PROGRAM) sendNormal CIM:CQL
	@$(ECHO) "TestProcessIndication checkNormal CIM:CQL"
	@$(PROGRAM) checkNormal CIM:CQL
	@$(ECHO) "TestProcessIndication sendSubclass CIM:CQL"
	@$(PROGRAM) sendSubclass CIM:CQL
	@$(ECHO) "TestProcessIndication checkSubclass CIM:CQL"
	@$(PROGRAM) checkSubclass CIM:CQL
	@$(ECHO) "TestProcessIndication sendMissing CIM:CQL"
	@$(PROGRAM) sendMissing CIM:CQL
	@$(ECHO) "TestProcessIndication checkMissing CIM:CQL"
	@$(PROGRAM) checkMissing CIM:CQL
	@$(ECHO) "TestProcessIndication sendExtra CIM:CQL"
	@$(PROGRAM) sendExtra CIM:CQL
	@$(ECHO) "TestProcessIndication checkExtra CIM:CQL"
	@$(PROGRAM) checkExtra CIM:CQL
	@$(ECHO) "TestProcessIndication sendMatching CIM:CQL"
	@$(PROGRAM) sendMatching CIM:CQL
	@$(ECHO) "TestProcessIndication checkMatching CIM:CQL"
	@$(PROGRAM) checkMatching CIM:CQL
	@$(ECHO) "TestProcessIndication sendUnmatchingNamespace CIM:CQL"
	@$(PROGRAM) sendUnmatchingNamespace CIM:CQL
	@$(ECHO) "TestProcessIndication checkUnmatchingNamespace CIM:CQL"
	@$(PROGRAM) checkUnmatchingNamespace CIM:CQL
	@$(ECHO) "TestProcessIndication sendUnmatchingClassName CIM:CQL"
	@$(PROGRAM) sendUnmatchingClassName CIM:CQL
	@$(ECHO) "TestProcessIndication checkUnmatchingClassName CIM:CQL"
	@$(PROGRAM) checkUnmatchingClassName CIM:CQL
	@$(ECHO) "TestProcessIndication delete1 CIM:CQL"
	@$(PROGRAM) delete1 CIM:CQL
	@$(ECHO) "TestProcessIndication create2 CIM:CQL"
	@$(PROGRAM) create2 CIM:CQL 
	@$(ECHO) "TestProcessIndication sendNormal CIM:CQL"
	@$(PROGRAM) sendNormal CIM:CQL
	@$(ECHO) "TestProcessIndication checkNormalAll CIM:CQL"
	@$(PROGRAM) checkNormalAll CIM:CQL
	@$(ECHO) "TestProcessIndication sendMissing CIM:CQL"
	@$(PROGRAM) sendMissing CIM:CQL
	@$(ECHO) "TestProcessIndication checkMissingAll CIM:CQL"
	@$(PROGRAM) checkMissingAll CIM:CQL
	@$(ECHO) "TestProcessIndication sendExtra CIM:CQL"
	@$(PROGRAM) sendExtra CIM:CQL
	@$(ECHO) "TestProcessIndication checkExtraAll CIM:CQL"
	@$(PROGRAM) checkExtraAll CIM:CQL
	@$(ECHO) "TestProcessIndication delete2 CIM:CQL"
	@$(PROGRAM) delete2 CIM:CQL
	@$(ECHO) "TestProcessIndication create3 CIM:CQL"
	@$(PROGRAM) create3 CIM:CQL
	@$(ECHO) "TestProcessIndication sendNormal CIM:CQL"
	@$(PROGRAM) sendNormal CIM:CQL
	@$(ECHO) "TestProcessIndication checkNormalWhere CIM:CQL"
	@$(PROGRAM) checkNormalWhere CIM:CQL
	@$(ECHO) "TestProcessIndication sendMissing CIM:CQL"
	@$(PROGRAM) sendMissing CIM:CQL
	@$(ECHO) "TestProcessIndication checkMissingWhere CIM:CQL"
	@$(PROGRAM) checkMissingWhere CIM:CQL
	@$(ECHO) "TestProcessIndication delete3 CIM:CQL"
	@$(PROGRAM) delete3 CIM:CQL
	@$(ECHO) "TestProcessIndication create4 CIM:CQL"
	@$(PROGRAM) create4 CIM:CQL
	@$(ECHO) "TestProcessIndication sendNormal CIM:CQL"
	@$(PROGRAM) sendNormal CIM:CQL
	@$(ECHO) "TestProcessIndication checkNormalWhereNotSatisfied CIM:CQL"
	@$(PROGRAM) checkNormalWhereNotSatisfied CIM:CQL
	@$(ECHO) "TestProcessIndication sendMissing CIM:CQL"
	@$(PROGRAM) sendMissing CIM:CQL
	@$(ECHO) "TestProcessIndication checkMissingWhere CIM:CQL"
	@$(PROGRAM) checkMissingWhere CIM:CQL
	@$(ECHO) "TestProcessIndication delete4 CIM:CQL"
	@$(PROGRAM) delete4 CIM:CQL
	@$(ECHO) "TestProcessIndication cleanup CIM:CQL"
	@$(PROGRAM) cleanup CIM:CQL
	@$(ECHO) "+++++ TestProcessIndication passed all tests"

IndInit_TS1a:
	@$(ECHO) "TestProcessIndication setup WQL"
	@$(PROGRAM) setup WQL
	@$(ECHO) "TestProcessIndication create1 WQL"
	@$(PROGRAM) create1 WQL
	@$(ECHO) "TestProcessIndication create2 WQL"
	@$(PROGRAM) create2 WQL

IndInit_TS1b:
	@$(ECHO) "TestProcessIndication delete1 WQL"
	@$(PROGRAM) delete1 WQL
	@$(ECHO) "TestProcessIndication delete2 WQL"
	@$(PROGRAM) delete2 WQL
	@$(ECHO) "TestProcessIndication cleanup WQL"
	@$(PROGRAM) cleanup WQL
