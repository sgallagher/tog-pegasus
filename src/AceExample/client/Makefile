ROOT=../../..
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) $(ACE_INCLUDES)

LIBRARIES = $(ACE_LIB)

SOURCES = test_client.cpp inst.cpp handler.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

PROGRAM = test_client

include $(ROOT)/mak/program.mak
