ROOT = ../../..
include $(ROOT)/mak/config.mak

SOURCE_DIR = Clients/DefaultC++/InvokeMethod

PROGRAM = InvokeMethod 
SOURCES = InvokeMethod.cpp

LIBRARIES = \
  pegcommon \
  pegclient

include $(ROOT)/mak/program.mak

tests:
	@$(SAMPLES_DIR)/$(SOURCE_DIR)/$(PROGRAM)
