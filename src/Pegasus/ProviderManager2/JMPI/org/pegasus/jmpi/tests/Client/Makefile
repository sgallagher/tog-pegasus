JAVAROOT = ../../../../..
ROOT = ../../../../../../../../..

include $(ROOT)/mak/config.mak

.SUFFIXES: .java .class .jar

.java.class:
	javac -classpath $(JAVAROOT) $*.java

PACKAGEDIR = org/pegasus/jmpi/tests/Client

CLASSES = \
   TestCIMClient.class

PFXCLASSES=$(addprefix -C ../../../../.. org/pegasus/jmpi/tests/Client/,$(CLASSES))

all: $(PEGASUS_HOME)/lib/Client.jar $(CLASSES)

clean:
	$(RM) *.class
	$(RM) $(PEGASUS_HOME)/lib/Client.jar

$(PEGASUS_HOME)/lib/Client.jar: $(CLASSES)
	jar -cf $@ $(PFXCLASSES)

