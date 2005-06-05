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
# Makefile builds a Sample Provider Namespace

ROOT = ../..

include $(ROOT)/mak/config.mak

INTEROPNS = root/PG_InterOp

SAMPLEPROVIDERNS=root/SampleProvider

createrepository:
	@ $(ECHO) +++++ Creating $(SAMPLEPROVIDERNS) namespace ...
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Physical$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA)/CIM_Physical$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_System$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA)/CIM_System$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -I$(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA) -n$(SAMPLEPROVIDERNS) $(PEGASUS_MOF_DIR)/$(PEGASUS_CIM_SCHEMA)/CIM_Event$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading class definitions into $(SAMPLEPROVIDERNS) namespace ...
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(SAMPLEPROVIDERNS) SampleProviderSchema.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(SAMPLEPROVIDERNS) CWS_FilesAndDir.mof

registerproviders:
	@ $(ECHO) +++++ Registering providers for SampleProvider namespace  ...
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) InstanceProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) MethodProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) IndicationProviderR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) SimpleDisplayConsumerR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) CWS_FilesAndDirR.mof
	@ $(PEGASUS_BIN_DIR)/cimmof -n$(INTEROPNS) AssociationProviderR.mof

deregisterproviders:
	@ $(ECHO) +++++ Deregistering SampleInstanceProvider ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleInstanceProviderModule
	@ $(ECHO) +++++ Deregistering SampleMethodProvider ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleMethodProviderModule
	@ $(ECHO) +++++ Deregistering SampleIndicationProvider ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleIndicationProviderModule
	@ $(ECHO) +++++ Deregistering SimpleDisplayConsumerModule ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SimpleDisplayConsumerModule
	@ $(ECHO) +++++ Deregistering FilesandDirectoryModule ... 	 
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m FilesAndDirectoriesModule
	@ $(ECHO) +++++ Deregistering SampleAssociationModule ...
	@ $(PEGASUS_BIN_DIR)/cimprovider -r -m SampleAssociationModule
