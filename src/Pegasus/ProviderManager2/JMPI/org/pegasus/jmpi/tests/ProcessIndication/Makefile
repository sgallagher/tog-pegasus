JAVAROOT = ../../../../..
ROOT = ../../../../../../../../..

include $(ROOT)/mak/config.mak
#include $(ROOT)/mak/program.mak

.SUFFIXES: .java .class .jar

.java.class:
	javac -classpath $(JAVAROOT) $*.java

PACKAGEDIR = org/pegasus/jmpi/tests/ProcessIndication

CLASSES1 = \
   JMPI_RT_SampleProvider1.class
JAR1 = \
   $(PEGASUS_HOME)/lib/JMPI_RT_SampleProvider1.jar

CLASSES2 = \
   JMPI_RT_SampleProvider2.class
JAR2 = \
   $(PEGASUS_HOME)/lib/JMPI_RT_SampleProvider2.jar

PROGRAM = TestJMPIIndicationSubscription

SOURCES = TestJMPIIndicationSubscription.cpp

FILES_TO_CLEAN = $(CLASSES1) $(JAR1) \
                 $(CLASSES2) $(JAR2)

all: $(CLASSES1) $(JAR1) $(CLASSES2) $(JAR2)

$(JAR1): $(CLASSES1)
	jar -cf $@ $(addprefix -C ../../../../.. org/pegasus/jmpi/tests/ProcessIndication/,$^)

$(JAR2): $(CLASSES2)
	jar -cf $@ $(addprefix -C ../../../../.. org/pegasus/jmpi/tests/ProcessIndication/,$^)

clean:
	$(RM) $(FILES_TO_CLEAN)
