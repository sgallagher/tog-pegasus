ROOT = ../../..
include $(ROOT)/mak/config.mak

PROGRAM = SendTestIndications
SOURCES = SendTestIndications.cpp

LIBRARIES = \
   pegcommon \
   pegclient

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
