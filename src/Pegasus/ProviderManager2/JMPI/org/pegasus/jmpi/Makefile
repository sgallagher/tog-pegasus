JAVAROOT = ../../..
ROOT = ../../../../../../..

.SUFFIXES: .java .class .jar

.java.class:
	javac -classpath $(JAVAROOT) $*.java

PACKAGEDIR = org/pegasus/jmpi

   CLASSES = \
   CIMInstance.class \
   CIMClass.class \
   CIMDataType.class \
   CIMDateTime.class \
   CIMException.class \
   CIMInstance.class \
   CIMMethod.class \
   CIMNameSpace.class \
   CIMObjectPath.class \
   CIMOMHandle.class \
   CIMProvider.class \
   CIMProperty.class \
   CIMValue.class \
   CIMQualifier.class \
   CIMDataType.class \
   InstanceProvider.class \
   MethodProvider.class \
   AssociatorProvider.class \
   UnsignedNumber.class \
   UnsignedInt16.class \
   UnsignedInt32.class \
   UnsignedInt64.class \
   UnsignedInt8.class \
   CIMNameSpace.class \
   CIMClient.class \
   InstEnumeration.class \
   PathEnumeration.class \
   JarClassLoader.class

PFXCLASSES=$(addprefix -C ../../.. org/pegasus/jmpi/,$(CLASSES))

all: $(PEGASUS_HOME)/lib/JMPIImpl.jar $(CLASSES)

clean:
	-rm *.class
	-rm $(PEGASUS_HOME)/lib/JMPIImpl.jar

$(PEGASUS_HOME)/lib/JMPIImpl.jar: $(CLASSES)
	jar -cf $@ $(PFXCLASSES) 

include $(ROOT)/mak/config.mak
