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
ifndef PEGASUS_ROOT
    $(error PEGASUS_ROOT environment variable undefined)
endif
ifndef PEGASUS_HOME
    $(error PEGASUS_HOME environment variable undefined)
endif

include $(PEGASUS_ROOT)/mak/config.mak
include $(PEGASUS_ROOT)/mak/configschema.mak
include $(PEGASUS_ROOT)/test/config.mak

.SUFFIXES: .java .class .jar

.java.class:
# Note: .. included in path to compile package helper classes if necessary
	javac -classpath $(PEGASUS_HOME)/lib/JMPIImpl.jar$(PEGASUS_JAVA_CLASSPATH_DELIMITER).. $*.java

KNOWN_BUGS       = BugExistsExecQuery

PACKAGEDIR       = Client/
JAR1             = JMPITestClient.jar
JAR1CLASSES      = \
	RunTest.class \
	TestEntry.class
EXTRAJAR1CLASSES = \
        MyFilenameFilter.class
JAR1MANIFEST     = JMPITestClient.manifest
JAR2             = JMPITestInstanceProvider.jar
JAR2CLASSES      = \
	testDataType.class \
	testExecQuery.class \
	testIndications.class \
	testPropertyProvider.class \
	testCIMClient.class \
	testCIMArgument.class \
	testCIMClass.class
EXTRAJAR2CLASSES =
JARS             = $(JAR1) $(JAR2)

LOCAL_CLEAN_TARGET = \
	$(JAR1CLASSES) $(EXTRAJAR1CLASSES) $(addprefix $(PEGASUS_HOME)/lib/,$(JAR1)) \
	$(JAR2CLASSES) $(EXTRAJAR2CLASSES) $(addprefix $(PEGASUS_HOME)/lib/,$(JAR2))

all build: 			$(addprefix $(PEGASUS_HOME)/lib/,$(JARS))

depend:

clean:
	$(RM) $(LOCAL_CLEAN_TARGET)

repository:

poststarttests:			$(addprefix $(PEGASUS_HOME)/lib/,$(JARS))
	java -classpath $(PEGASUS_HOME)/lib/JMPIImpl.jar$(PEGASUS_JAVA_CLASSPATH_DELIMITER)$(addprefix $(PEGASUS_HOME)/lib/,$(JAR1))$(PEGASUS_JAVA_CLASSPATH_DELIMITER)$(addprefix $(PEGASUS_HOME)/lib/,$(JAR2)) Client.RunTest $(KNOWN_BUGS)

$(PEGASUS_HOME)/lib/$(JAR1):	$(JAR1CLASSES) $(JAR1MANIFEST)
	jar -cfm $@ $(JAR1MANIFEST) $(addprefix -C .. $(PACKAGEDIR),$^) $(addprefix -C .. $(PACKAGEDIR),$(EXTRAJAR1CLASSES))

$(PEGASUS_HOME)/lib/$(JAR2):	$(JAR2CLASSES)
	jar -cf $@ $(addprefix -C .. $(PACKAGEDIR),$^) $(addprefix -C .. $(PACKAGEDIR),$(EXTRAJAR2CLASSES))

#include $(ROOT)/test/configend.mak
