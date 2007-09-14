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
# Makefile builds a Sample Provider Namespace

ROOT = ../..
include $(ROOT)/mak/config.mak

INTEROPNS = root/PG_InterOp

SAMPLECXXPROVIDERNS=SDKExamples/DefaultCXX
SAMPLECMPIPROVIDERNS=SDKExamples/CMPI

createCXXrepository:
	@$(ECHO) "+++++ Creating $(SAMPLECXXPROVIDERNS) namespace ..."
	@$(ECHO) "+++++ Loading SDK_CIM_Schema.mof into" \
            "$(SAMPLECXXPROVIDERNS) namespace ..."
	@$(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_CIM_SCHEMA_DIR) \
            -n$(SAMPLECXXPROVIDERNS) SDK_CIM_Schema.mof
	@ $(ECHO) "+++++ Loading class definitions into " \
            "$(SAMPLECXXPROVIDERNS) namespace ..."
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(SAMPLECXXPROVIDERNS) \
             SampleProviderSchema.mof

createCMPIrepository:
	@$(ECHO) "+++++ Creating $(SAMPLECMPIPROVIDERNS) namespace ..."
	@$(ECHO) "+++++ Loading SDK_CIM_Schema.mof into" \
            "$(SAMPLECMPIPROVIDERNS) namespace ..."
	@$(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_CIM_SCHEMA_DIR) \
            -n$(SAMPLECMPIPROVIDERNS) SDK_CIM_Schema.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(SAMPLECMPIPROVIDERNS) CWS_FilesAndDir.mof

createrepository: createCXXrepository createCMPIrepository
	@$(ECHO) "+++++ Repository created."

registerproviders:
	@ $(ECHO) "+++++ Registering providers for" \
            "$(SAMPLECXXPROVIDERNS) namespace  ..."
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) InstanceProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) MethodProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) IndicationProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) SimpleDisplayConsumerR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) CWS_FilesAndDirR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) AssociationProviderR.mof

deregisterproviders:
	@ $(ECHO) "+++++ Deregistering SampleInstanceProvider ..."
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleInstanceProviderModule
	@ $(ECHO) "+++++ Deregistering SampleMethodProvider ..."
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleMethodProviderModule
	@ $(ECHO) "+++++ Deregistering SampleIndicationProvider ..."
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleIndicationProviderModule
	@ $(ECHO) "+++++ Deregistering SimpleDisplayConsumerModule ..."
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SimpleDisplayConsumerModule
	@ $(ECHO) "+++++ Deregistering FilesandDirectoryModule ..." 	 
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m FilesAndDirectoriesModule
	@ $(ECHO) "+++++ Deregistering SampleAssociationModule ..."
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleAssociationModule
