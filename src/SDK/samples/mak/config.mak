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
ECHO = echo

VALID_PLATFORMS = \
    WIN32_IX86_MSVC \
    LINUX_IX86_GNU \
    LINUX_PPC_GNU \
    LINUX_PPC64_GNU \
    LINUX_IA64_GNU \
    LINUX_X86_64_GNU \
    LINUX_ZSERIES_GNU \
    LINUX_ZSERIES64_GNU \
    AIX_RS_IBMCXX \
    HPUX_PARISC_ACC \
    HPUX_IA64_ACC \
    TRU64_ALPHA_DECCXX \
    SOLARIS_SPARC_GNU \
    SOLARIS_SPARC_CC \
    ZOS_ZSERIES_IBM \
    NSK_NONSTOP_NMCPLUS

ifndef PEGASUS_PLATFORM
  ERROR = pegasus_undefined
pegasus_undefined:
	@ $(ECHO) PEGASUS_PLATFORM environment variable undefined. Please set to\
	one of the following: $(VALID_PLATFORMS)
	@ exit 1
endif

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_PPC_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_PPC64_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IA64_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_X86_64_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_ZSERIES_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_ZSERIES64_GNU)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_IA64_ACC)
  include $(ROOT)/mak/$(PEGASUS_PLATFORM).mak
endif
