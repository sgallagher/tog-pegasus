# Makefile for CMPISampleClass library

# Change the following lines to match the values on your system
PEGDIRLIB=$(PEGASUS_HOME)/lib

# Changes shouldn't be necessary below this line
CFLAGS=-Wall -D_COMPILE_UNIX -g -I. -I$(PEGASUS_HOME)/src
LDFLAGS= -shared -ldl -lpthread

lib%.so: %.o
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

all: test libCMPISampleClass.so

install: all
	install libCMPISampleClass.so $(PEGDIRLIB)
	cimmofl -I. -nroot/cimv2 CMPISampleClass.mof
	cimmofl -I. -nroot/PG_InterOp CMPISampleClassR.mof

clean:
	$(RM) *.so *.o

test:
	@[ -d $(PEGDIRLIB) ] || ( echo $(PEGDIRLIB) does not exist && false)
