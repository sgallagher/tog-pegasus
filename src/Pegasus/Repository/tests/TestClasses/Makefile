ROOT = ../../../../..
DIR = Pegasus/Repository/TestClasses
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = TestClasses
SOURCES = TestClasses.cpp

include $(ROOT)/mak/program.mak

tests:
	@ TestClasses $(REPOSITORY_ROOT)
	@ $(RM) tmpfile
