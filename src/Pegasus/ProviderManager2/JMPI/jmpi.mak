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
ROOT = ../../../..

DIR = Pegasus/ProviderManager2/JMPI

include $(ROOT)/mak/config.mak

ifeq ($(OS_TYPE),windows)
   JAVALIBS=$(JAVA_SDK)/jre/lib/
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include -I$(JAVA_SDK)/include/win32
   EXTRA_LIBRARIES += $(JAVA_SDK)/lib/jvm.lib
else
ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
   SYS_INCLUDES += -I${JAVA_SDK}/include
   EXTRA_LIBRARIES += ${JAVA_SDK}/bin/classic/libjvm.x
else
ifndef PEGASUS_JVM
   PEGASUS_JVM=sun
endif
ifeq ($(PEGASUS_JVM),sun)
   JAVALIBS=$(JAVA_SDK)/jre/lib/$(PEGASUS_JAVA_ARCH)
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include -I$(JAVA_SDK)/include/linux
   EXTRA_LIBRARIES += -L$(JAVALIBS)/native_threads -L$(JAVALIBS)/client -ljvm -lhpi -lcrypt
endif
ifeq ($(PEGASUS_JVM),ibm)
   JAVALIBS=$(JAVA_SDK)/jre/bin
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include
   EXTRA_LIBRARIES += -L$(JAVALIBS)/classic/ -L$(JAVALIBS)/ -ljvm -lhpi -lcrypt
endif
ifeq ($(PEGASUS_JVM),bea)
   JAVALIBS=$(JAVA_SDK)/jre/lib/$(PEGASUS_JAVA_ARCH)
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include/ -I$(JAVA_SDK)/include/linux/
   EXTRA_LIBRARIES += -L$(JAVALIBS)/ -L$(JAVALIBS)/jrockit/ -L$(JAVALIBS)/native_threads/ -ljvm -lhpi -lcrypt
endif
ifeq ($(PEGASUS_JVM),gcj)
   JAVALIBS=$(JAVA_SDK)/jre/lib/$(PEGASUS_JAVA_ARCH)
   EXTRA_LIBRARIES += -L$(JAVALIBS)/client -ljvm
endif
endif
endif

LOCAL_DEFINES = -DPEGASUS_JMPIPM_INTERNAL -DPEGASUS_INTERNALONLY

ifeq ($(OS_TYPE),vms)
 EXTRA_LIBRARIES += java\$jvm_shr
 VMS_VECTOR = PegasusCreateProviderManager
endif

LIBRARY = JMPIProviderManager

LIBRARIES = \
	pegprovidermanager \
	pegconfig \
	pegwql \
	pegquerycommon \
	pegprovider \
	pegclient \
	pegcommon

SOURCES = \
        JMPIProviderManagerMain.cpp \
        JMPIProviderManager.cpp \
        JMPILocalProviderManager.cpp \
        JMPIProviderModule.cpp \
        JMPIProvider.cpp \
        JMPIImpl.cpp

include $(ROOT)/mak/dynamic-library.mak

ifeq ($(OS_TYPE),vms)
all:    $(FULL_PROGRAM)
else
all:    $(FULL_LIB)
endif

repository tests poststarttests:
