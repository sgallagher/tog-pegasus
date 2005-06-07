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
JAVAROOT = ../../..
ROOT = ../../../../../../..

include $(ROOT)/mak/config.mak

.SUFFIXES: .java .class .jar

.java.class:
	javac -classpath $(JAVAROOT) $*.java

PACKAGEDIR = org/pegasus/jmpi

   CLASSES = \
   CIMClass.class \
   CIMInstance.class \
   CIMDataType.class \
   CIMDateTime.class \
   CIMElement.class \
   CIMException.class \
   CIMInstanceException.class \
   CIMInstance.class \
   CIMMethod.class \
   CIMNameSpace.class \
   CIMObjectPath.class \
   CIMOMHandle.class \
   ProviderCIMOMHandle.class \
   CIMProperty.class \
   CIMValue.class \
   CIMQualifier.class \
   CIMQualifierType.class \
   CIMDataType.class \
   CIMFlavor.class \
   CIMScope.class \
   CIMArgument.class \
   \
   CIMProvider.class \
   CIMInstanceProvider.class \
   CIMAssociatorProvider.class \
   CIMMethodProvider.class \
   \
   InstanceProvider.class \
   MethodProvider.class \
   AssociatorProvider.class \
   EventProvider.class \
   PropertyProvider.class \
   \
   SelectExp.class \
   FromExp.class \
   NonJoinExp.class \
   SelectList.class \
   QueryExp.class \
   JMPIQueryExp.class \
   WQLExp.class \
   \
   UnsignedNumber.class \
   UnsignedInt16.class \
   UnsignedInt32.class \
   UnsignedInt64.class \
   UnsignedInt8.class \
   \
   CIMNameSpace.class \
   CIMClient.class \
   InstEnumeration.class \
   PathEnumeration.class \
   QualEnumeration.class \
   \
   JarClassLoader.class

PFXCLASSES=$(addprefix -C ../../.. org/pegasus/jmpi/,$(CLASSES))

all: $(PEGASUS_HOME)/lib/JMPIImpl.jar $(CLASSES)

clean:
	-$(RM) *.class
	-$(RM) $(PEGASUS_HOME)/lib/JMPIImpl.jar

$(PEGASUS_HOME)/lib/JMPIImpl.jar: $(CLASSES)
	jar -cf $@ $(PFXCLASSES)

