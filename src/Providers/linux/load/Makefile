#//%////////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2002 BMC Software, Hewlett-Packard Company, IBM,
#// The Open Group, Tivoli Systems
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
#//
#// Author: Al Stone <ahs3@fc.hp.com>
#//
#//%////////////////////////////////////////////////////////////////////////////
#
# Makefile loads the Linux_CDROMDrive provider

ROOT = ../../../..
include $(ROOT)/mak/config.mak

MOF_PATH = .
CIMMOF = $(PEGASUS_HOME)/bin/cimmof
NAMESPACE = root/cimv2
INTEROPNAMESPACE = root/cimv2

MOFS = \
   CDROMDriveProvider.mof \
   DiskDriveProvider.mof \
   IOPortProvider.mof \
   IPRouteProvider.mof \
   InterruptProvider.mof \
   NetworkAdapterProvider.mof \
   OperatingSystemProvider.mof \
   PCIControllerProvider.mof \
   ProcessorProvider.mof \
   SoftwareElementProvider.mof

REGISTRATIONMOFS = \
   CDROMDriveProviderRegistration.mof \
   DiskDriveProviderRegistration.mof \
   IOPortProviderRegistration.mof \
   IPRouteProviderRegistration.mof \
   OperatingSystemProviderRegistration.mof \
   ProcessorProviderRegistration.mof \
   PCIControllerProviderRegistration.mof \
   InterruptProviderRegistration.mof \
   NetworkAdapterProviderRegistration.mof \
   SoftwareElementProviderRegistration.mof

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ $(foreach i, $(MOFS), echo +++++ Loading $(i) into $(NAMESPACE) namespace ...; cimmof -I$(MOF_PATH) -n$(NAMESPACE) $(MOF_PATH)/$(i);)

register:
	@ $(foreach i, $(REGISTRATIONMOFS), echo +++++ Loading $(i) into $(INTEROPNAMESPACE) namespace ...; cimmof -I$(MOF_PATH) -n$(INTEROPNAMESPACE) $(MOF_PATH)/$(i);)

clean:

depend:

sub:

misc:

tests:
