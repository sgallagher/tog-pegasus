JAVAROOT = ../../../../..
ROOT = ../../../../../../../../..

include $(ROOT)/mak/commands.mak

.SUFFIXES: .java .class .jar

.java.class:
	javac -classpath $(JAVAROOT) $*.java

PACKAGEDIR = org/pegasus/jmpi/tests/ProcessIndication

CLASSES = \
   JMPI_RT_SampleProvider.class

PFXCLASSES=$(addprefix -C ../../../../.. org/pegasus/jmpi/tests/ProcessIndication/,$(CLASSES))

all: $(PEGASUS_HOME)/lib/JMPI_RT_SampleProvider.jar $(CLASSES)

clean:
	$(RM) *.class
	$(RM) $(PEGASUS_HOME)/lib/JMPI_RT_SampleProvider.jar

$(PEGASUS_HOME)/lib/JMPI_RT_SampleProvider.jar: $(CLASSES)
	jar -cf $@ $(PFXCLASSES)

include $(ROOT)/mak/config.mak
