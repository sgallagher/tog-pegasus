JAVAROOT = ../../../../..
ROOT = ../../../../../../../../..

.SUFFIXES: .java .class .jar

.java.class:
	javac -classpath $(JAVAROOT) $*.java

PACKAGEDIR = org/pegasus/jmpi/tests/JMPI_TestPropertyTypes

   CLASSES = \
   JMPI_TestPropertyTypes.class

PFXCLASSES=$(addprefix -C ../../../../.. org/pegasus/jmpi/tests/JMPI_TestPropertyTypes/,$(CLASSES))

all: $(PEGASUS_HOME)/lib/JMPI_TestPropertyTypes.jar $(CLASSES)

clean:
	-rm *.class
	-rm $(PEGASUS_HOME)/lib/JMPI_TestPropertyTypes.jar

$(PEGASUS_HOME)/lib/JMPI_TestPropertyTypes.jar: $(CLASSES)
	jar -cf $@ $(PFXCLASSES)

include $(ROOT)/mak/config.mak
