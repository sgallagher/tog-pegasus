ROOT = ../../../../..
DIR = Pegasus/Repository/LoadRepository
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = LoadRepository

SOURCES = LoadRepository.cpp

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) $(REPOSITORY_ROOT)
	LoadRepository $(REPOSITORY_ROOT) x.xml
