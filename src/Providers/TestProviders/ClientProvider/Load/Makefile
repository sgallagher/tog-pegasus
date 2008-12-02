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
# Makefile builds a Test Provider Namepspace
# Note: For best results, ensure that the root/TestProvider namespace does not
#       exist before calling this Makefile.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

# The following variable determine the version of the Schema to
# be loaded into the Test Provider namespace.
# Update the following variable to change the version.

CIM_SCHEMA_VER=25
CIM_SCHEMA_DIR=CIM$(CIM_SCHEMA_VER)
CIM_MOF_PATH = $(ROOT)/Schemas/$(CIM_SCHEMA_DIR)

TESTPROVIDERNS=root/TestProvider

repository:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_MOF_PATH)" "-n$(TESTPROVIDERNS)" $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ $(ECHO) +++++ Loading TestProvider class definitions into $(TESTPROVIDERNS) namespace ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(TESTPROVIDERNS)" ClientProviderSchema.mof
	@ $(ECHO) +++++ Registering TestProvider schema Providers  ...
	@ cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-n$(INTEROPNS)" ClientProviderSchemaR.mof

repositoryServer:
	@ $(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(TESTPROVIDERNS) namespace ...
	@ cimmof "-I$(CIM_MOF_PATH)" "-n$(TESTPROVIDERNS)" $(CIM_MOF_PATH)/CIM_Core$(CIM_SCHEMA_VER).mof
	@ cimmof "-n$(TESTPROVIDERNS)" ClientProviderSchema.mof
	@ cimmof "-n$(INTEROPNS)" ClientProviderSchemaR.mof

unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m TestClientProviderModule

depend:

sub:

misc:

tests:

poststarttests:

general:

clean:
