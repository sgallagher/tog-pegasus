ROOT = $(PEGASUS_ROOT)

PROGRAM_NAME = loadxml
 
DIR = Clients/$(PROGRAM_NAME)
include $(ROOT)/mak/config.mak
include ../libraries.mak

PROGRAM = $(PROGRAM_NAME)

SOURCES = $(PROGRAM_NAME).cpp

LIBRARIES += pegrepository

include $(ROOT)/mak/program.mak

tests:
	@ echo remove repository at $(REPOSITORY_PATH)
	$(RMREPOSITORY) $(REPOSITORY_PATH)/repository

	$(PROGRAM_NAME)  x.xml root $(REPOSITORY_ROOT)
	
poststarttests:

