JAVAROOT = ../../../../..
ROOT = ../../../../../../../../..

.SUFFIXES: .java .class .jar

.java.class:
	javac -classpath $(JAVAROOT) $*.java

PACKAGEDIR = org/pegasus/jmpi/tests/ProcessIndication

CLASSES = \
   JMPI_RT_SampleProvider.class

PFXCLASSES=$(addprefix -C ../../../../.. org/pegasus/jmpi/tests/ProcessIndication/,$(CLASSES))

all: $(PEGASUS_HOME)/lib/JMPI_RT_SampleProvider.jar $(CLASSES)

clean:
	-rm *.class
	-rm $(PEGASUS_HOME)/lib/JMPI_RT_SampleProvider.jar

$(PEGASUS_HOME)/lib/JMPI_RT_SampleProvider.jar: $(CLASSES)
	jar -cf $@ $(PFXCLASSES)

include $(ROOT)/mak/config.mak
