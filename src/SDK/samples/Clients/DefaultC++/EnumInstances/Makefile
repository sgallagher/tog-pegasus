ROOT = ../../..
include $(ROOT)/mak/config.mak

SOURCE_DIR = Clients/DefaultC++/EnumInstances

PROGRAM = EnumInstances
SOURCES = EnumInstances.cpp

LIBRARIES = \
   pegcommon \
   pegclient

include $(ROOT)/mak/program.mak

tests:
	@$(SAMPLES_DIR)/$(SOURCE_DIR)/$(PROGRAM)
