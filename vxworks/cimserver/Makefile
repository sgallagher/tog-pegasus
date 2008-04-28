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
#//=============================================================================

# This Makefile is an example implementation of an embedded system
# cim server make for VxWorks.
# This Makefile builds the repositories and main module. This Makefile should
# be treated an an example for a particular embedded server implementation.

ROOT = ../..
DIR = vxworks/cimserver
include $(ROOT)/mak/config.mak

PROGRAM = cimserver

#
# The sources include the main module (cimserver) and the
# cimmofl generated files for each namespace to be installed
#
SOURCES = \
    cimserver.cpp \
    EmbeddedServer.cpp \
    MyEmbeddedServer.cpp \
    root_cimv2_namespace.cpp \
    root_PG_Internal_namespace.cpp \
    root_PG_InterOp_namespace.cpp

# The libraries include the pegasus functions AND the providers
# that were created for this embedded cim server.
# Note that GoodbyeProvider and HelloProvider are simple example 
# providers

LIBRARIES = \
    cimplepegadap \
    cimplepegutils \
    cimple \
    EmployeeProvider \
    pegcimserver \
    pegserver \
    pegservice \
    peguser \
    NamespaceProvider \
    pegindicationservice \
    peghandlerservice \
    ConfigSettingProvider \
    DefaultProviderManager \
    pegauthentication \
    UserAuthProvider \
    pegqueryexpression \
    pegcql \
    pegquerycommon \
    pegwql \
    CIMQueryCapabilitiesProvider \
    DefaultProviderManager \
    pegprovidermanager \
    CIMOMStatDataProvider \
    InteropProvider \
    ProviderRegistrationProvider \
    pegprm \
    pegprovidermanager \
    pegpmservice \
    pegprovider \
    pegexportserver \
    pegrepository \
    pegconfig \
    pegcommon

## Removing the provider registration capability removes the following
##    ProviderRegistrationProvider \
##    pegprm \

include $(ROOT)/mak/program.mak

##==============================================================================
##
## repository: Builds the repository
##
##==============================================================================

SCHEMAS=$(PEGASUS_ROOT)/Schemas

# set the following parameters for the compiler
#  -d - TBD 
#  -W - suppress the cimmofl usage warning message
#  -m - Compile classes to source files for memory-resident repository

CIMMOFL=cimmofl -d -W -m

# build the root/PG_Internal namespace into the file root_PG_Internal namespace
# The input is all of the mof to be included in this namespace.
repository:
	@ $(CIMMOFL) -n root/PG_Internal -I $(SCHEMAS) root_PG_Internal_namespace.mof
	@ $(CIMMOFL) -n root/PG_InterOp -I $(SCHEMAS) root_PG_InterOp_namespace.mof
	@ $(CIMMOFL) -n root/cimv2 -I $(SCHEMAS) root_cimv2_namespace.mof

share:
	cp $(BIN_DIR)/$(PROGRAM) /share
