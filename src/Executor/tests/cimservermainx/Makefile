ROOT = $(PEGASUS_ROOT)
DIR = cimservermainx/cimservermainx

include $(ROOT)/mak/config.mak

PROGRAM = cimservermainx
SOURCES = cimservermainx.cpp
EXTRA_INCLUDES = -I..
LIBRARIES = pegexecclient pegcommon

include $(ROOT)/mak/program.mak

tests:

poststarttests:

run:
	cimservermainx
