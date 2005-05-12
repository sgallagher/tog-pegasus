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
#//
#// Author: Christopher Neufeld <neufeld@linuxcare.com>
#//         David Kennedy       <dkennedy@linuxcare.com>
#//
#// Modified By: David Kennedy       <dkennedy@linuxcare.com>
#//              Christopher Neufeld <neufeld@linuxcare.com>
#//              Al Stone            <ahs3@fc.hp.com>
#//
#//%////////////////////////////////////////////////////////////////////////////


ROOT = ../../../..

PROVIDERROOT = $(ROOT)/src/Providers/linux
PROVIDERINCLUDE_ROOT = $(PROVIDERROOT)/Include
PROVIDERSUPPORT_ROOT = $(PROVIDERROOT)/ProviderSupport
PROVIDERDATA_ROOT    = $(PROVIDERROOT)/ProviderData

DIR = Providers/linux/Interrupt

include $(ROOT)/mak/config.mak

LIBRARY = LinuxInterruptProvider

EXTRA_INCLUDES = -I$(PROVIDERDATA_ROOT)/Interrupt -I$(PROVIDERDATA_ROOT) \
	-I$(PROVIDERINCLUDE_ROOT) -I$(PROVIDERSUPPORT_ROOT)/DeviceLocator

SOURCES = \
	InterruptProviderMain.cpp \
	InterruptProvider.cpp

LIBRARIES = \
	pegcommon \
	pegprovider \
	InterruptData \
	DeviceLocator \
	FileReader

DYNAMIC_LIBRARIES = \
	-lpegcommon \
	-lpegprovider \
	-lInterruptData \
	-lDeviceLocator \
	-lFileReader

include $(ROOT)/mak/library.mak

tests:

