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

ROOT = ../../../..
DIR = embedded/cimserver1
include $(ROOT)/mak/config.mak

PROGRAM = cimserver1

#
# The sources include the main module (cimserver) and the
# cimmofl generated files for each namespace to be installed
#
SOURCES = \
    cimserver.cpp \
    root_cimv2_namespace.cpp \
    root_PG_Internal_namespace.cpp \
    root_PG_InterOp_namespace.cpp

LIBRARIES = \
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

include $(ROOT)/mak/program.mak
