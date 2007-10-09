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

ROOT = ../..
DIR = vxworks/cimserver
include $(ROOT)/vxworks/config.mak
include $(ROOT)/mak/config.mak

PROGRAM = cimserver

SOURCES = \
    cimserver.cpp \
    root_cimv2_namespace.cpp \
    root_PG_Internal_namespace.cpp \
    root_PG_InterOp_namespace.cpp

LIBRARIES = \
    GoodbyeProvider \
    HelloProvider \
    pegcimserver \
    pegserver \
    pegservice \
    peguser \
    pegprm \
    NamespaceProvider \
    pegindicationservice \
    peghandlerservice \
    ConfigSettingProvider \
    DefaultProviderManager \
    ProviderRegistrationProvider \
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
    pegprovidermanager \
    pegpmservice \
    pegprovider \
    pegexportserver \
    pegrepository \
    pegconfig \
    pegclient \
    pegcommon

include $(ROOT)/mak/program.mak

install:
	cp $(PEGASUS_HOME)/bin/cimserver $(PEGASUS_HOME)/romfs

##==============================================================================
##
## rep:
##
##==============================================================================

SCHEMAS=$(PEGASUS_ROOT)/Schemas
CIMMOFL=cimmofl -d -W -m
#CIMMOFL=cimmofl -W -m

rep:
	@ $(CIMMOFL) -n root/PG_Internal -I $(SCHEMAS) root_PG_Internal.mof
	@ $(CIMMOFL) -n root/PG_InterOp -I $(SCHEMAS) root_PG_InterOp.mof
	@ $(CIMMOFL) -n root/cimv2 -I $(SCHEMAS) root_cimv2.mof

rep1:
	@ $(CIMMOFL) -n root/cimv2 -I $(SCHEMAS) root_cimv2.mof

rep2:
	@ $(CIMMOFL) -n root/PG_Internal -I $(SCHEMAS) root_PG_Internal.mof

rep3:
	@ $(CIMMOFL) -n root/PG_InterOp -I $(SCHEMAS) root_PG_InterOp.mof

