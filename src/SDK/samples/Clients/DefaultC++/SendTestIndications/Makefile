ROOT = ../../..
include $(ROOT)/mak/config.mak

SOURCE_DIR = Clients/DefaultC++/SendTestIndications

PROGRAM = SendTestIndications
SOURCES = SendTestIndications.cpp

LIBRARIES = \
   pegcommon \
   pegclient

include $(ROOT)/mak/program.mak

tests:
	@$(PEGASUS_SAMPLES_DIR)/$(SOURCE_DIR)/$(PROGRAM)
