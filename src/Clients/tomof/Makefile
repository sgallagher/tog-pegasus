ROOT = $(PEGASUS_ROOT)

PROGRAM_NAME = tomof
DIR = Clients/$(PROGRAM_NAME)
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = $(PROGRAM_NAME)

SOURCES = $(PROGRAM_NAME).cpp \
	  clientRepositoryInterface.cpp

LIBRARIES += pegrepository

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(RMDIRHIER) $(TMP_DIR)/repository
	$(PROGRAM)
	$(RMDIRHIER) $(TMP_DIR)/repository
