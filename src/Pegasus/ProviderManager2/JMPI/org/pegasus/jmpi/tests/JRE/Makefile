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
ifndef PEGASUS_ROOT
    $(error PEGASUS_ROOT environment variable undefined)
endif
ifndef PEGASUS_HOME
    $(error PEGASUS_HOME environment variable undefined)
endif

DIR = Pegasus/ProviderManager2/JMPI/org/pegasus/jmpi/tests/JRE

include $(PEGASUS_ROOT)/mak/config.mak

ifeq ($(OS_TYPE),windows)
   JAVALIBS=$(JAVA_SDK)/jre/lib/
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include -I$(JAVA_SDK)/include/win32
   EXTRA_LIBRARIES += $(JAVA_SDK)/lib/jvm.lib
else
ifeq ($(OS),zos)
   SYS_INCLUDES += -I${JAVA_SDK}/include
   EXTRA_LIBRARIES += ${JAVA_SDK}/bin/classic/libjvm.x
else
ifndef PEGASUS_JVM
   PEGASUS_JVM=sun
endif
ifeq ($(PEGASUS_JVM),sun)
   JAVALIBS=$(JAVA_SDK)/jre/lib/$(PEGASUS_JAVA_ARCH)
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include -I$(JAVA_SDK)/include/linux
   EXTRA_LIBRARIES += -L$(JAVALIBS)/native_threads -L$(JAVALIBS)/$(PEGASUS_JAVA_TYPE) -ljvm -lhpi -lcrypt
endif
ifeq ($(PEGASUS_JVM),ibm)
   JAVALIBS=$(JAVA_SDK)/jre/bin
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include
   EXTRA_LIBRARIES += -L$(JAVALIBS)/classic/ -L$(JAVALIBS)/ -ljvm -lhpi -lcrypt
endif
ifeq ($(PEGASUS_JVM),ibm64)
   JAVALIBS=$(JAVA_SDK)/jre/bin
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include
   EXTRA_LIBRARIES += -L$(JAVALIBS)/j9vm/ -L$(JAVALIBS)/classic/ -L$(JAVALIBS)/ -ljvm
endif
ifeq ($(PEGASUS_JVM),bea)
   JAVALIBS=$(JAVA_SDK)/jre/lib/$(PEGASUS_JAVA_ARCH)
   EXTRA_INCLUDES = $(SYS_INCLUDES) -I$(JAVA_SDK)/include/ -I$(JAVA_SDK)/include/linux/
   EXTRA_LIBRARIES += -L$(JAVALIBS)/ -L$(JAVALIBS)/jrockit/ -L$(JAVALIBS)/native_threads/ -ljvm -lhpi -lcrypt
endif
ifeq ($(PEGASUS_JVM),gcj)
   JAVALIBS=$(JAVA_SDK)/jre/lib/$(PEGASUS_JAVA_ARCH)
   EXTRA_LIBRARIES += -L$(JAVALIBS)/$(PEGASUS_JAVA_TYPE) -ljvm
endif
endif
endif

LIBRARIES = \
     pegcommon \
     pegconfig \
     JMPIProviderManager

LOGFILES = TestJRE

EXTRA_INCLUDES += -I$(PEGASUS_ROOT)/src/Pegasus/ProviderManager2/JMPI/

EXTRA_LINK_FLAGS += $(JVM_LINK_FLAGS)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

SOURCES = \
	TestJRE.cpp

PROGRAM = TestJRE

include $(ROOT)/mak/program.mak
include $(ROOT)/test/config.mak

ifeq ($(OS_TYPE),windows)
  SYS_LIBS = ws2_32.lib advapi32.lib	
endif

tests:

poststarttests: $(FULL_PROGRAM)
#	@$(RM) $(LOGFILES).log
#	@$(ECHO) $(PROGRAM) test
	@$(PROGRAM) test
#	@$(ECHO) +++ $(PROGRAM) passed successfully +++
#	@$(DIFF) $(LOGFILES).loggood $(LOGFILES).log
#	@$(ECHO) +++ $(LOGFILES) passed successfully +++

install:

repository:

all: $(FULL_PROGRAM)
