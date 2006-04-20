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
JAVAROOT = ../../../../..
ROOT     = ../../../../../../../../..
DIR      = Pegasus/ProviderManager2/JMPI/org/pegasus/jmpi/tests/Indication

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

.SUFFIXES: .java .class .jar .cpp

.java.class:
	javac -classpath $(JAVAROOT) $*.java

STATICTESTNS=test/static
STATICTESTNSDIRECTORY=$(REPOSITORY_ROOT)/test\#static

PACKAGEDIR = org/pegasus/jmpi/tests/Indication/

JAR1         = $(PEGASUS_HOME)/lib/JMPI_IndicationProvider.jar
JAR1CLASSES  = \
	JMPI_IndicationProvider.class
JAR2         = $(PEGASUS_HOME)/lib/JMPI_IndicationClient.jar
JAR2CLASSES  = \
	TestIndications.class
JAR2MANIFEST = TestIndications.manifest

PFXCLASSES   = $(addprefix -C ../../../../.. $(PACKAGEDIR),$(CLASSES))

LOCAL_CLEAN_TARGET = $(JAR1CLASSES) $(JAR2CLASSES) $(JAR1) $(JAR2)

all build: $(JAR1) $(JAR2)
	$(MAKE) -f TestIndicationsListener.Makefile

depend:

clean:
	$(RM) $(JAR1CLASSES) $(JAR2CLASSES)
	$(RM) $(JAR1) $(JAR2)
	$(MAKE) -f TestIndicationsListener.Makefile clean

repository:
	@$(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(STATICTESTNS) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(STATICTESTNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@$(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(STATICTESTNS) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(STATICTESTNS)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@$(ECHO) +++++ Loading $(JAR) into $(STATICTESTNS) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(STATICTESTNS)" JMPI_IndicationProvider.mof
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(INTEROPNS)" JMPI_IndicationProviderR.mof

poststarttests: $(JAR1) $(JAR2)
	java -jar $(JAR2) debug

$(JAR1): $(JAR1CLASSES)
	jar -cf $@ $(addprefix -C ../../../../.. $(PACKAGEDIR),$(JAR1CLASSES))

$(JAR2): $(JAR2CLASSES)
	jar -cfm $@ $(JAR2MANIFEST) $(addprefix -C ../../../../.. $(PACKAGEDIR),$(JAR2CLASSES))
