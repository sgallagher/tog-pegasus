ROOT = ../../..
include $(ROOT)/mak/config.mak

PROGRAM = SendTestIndications
SOURCES = SendTestIndications.cpp

LIBRARIES = \
   pegclient \
   pegcommon 

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
