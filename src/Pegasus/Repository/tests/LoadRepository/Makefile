ROOT = ../../../../..
DIR = Pegasus/Repository/LoadRepository
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = LoadRepository

SOURCES = LoadRepository.cpp

LOCAL_CLEAN_TARGET = local_clean_target

include $(ROOT)/mak/program.mak

tests:
	$(RMDIRHIER) $(REPOSITORY_ROOT)
	LoadRepository $(REPOSITORY_ROOT) $(ROOT)/doc/CIM_Schema24.xml
