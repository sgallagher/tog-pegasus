# Makefile for CMPIRT_SampleClass library

# Change the following lines to match the values on your system
PEGDIRLIB=$(PEGASUS_HOME)/lib

# Changes shouldn't be necessary below this line
CFLAGS=-Wall -D_COMPILE_UNIX -g -I. -I../..
LDFLAGS= -shared -ldl -lpthread

lib%.so: %.o
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

all: test libCMPI_RT_Sample.so

install: all
	install libCMPI_RT_Sample.so $(PEGDIRLIB)
	cimmofl "-I." "-nroot/cimv2" RT_Sample.mof
	cimmofl "-I." "-nroot/PG_InterOp" RT_SampleR.mof

clean:
	$(RM) *.so *.o

test:
	@[ -d $(PEGDIRLIB) ] || ( $(ECHO) $(PEGDIRLIB) does not exist && false)
