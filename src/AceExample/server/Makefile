ROOT=../../..

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LIBRARIES = $(ACE_LIB)

SOURCES = test_server.cpp inst.cpp handler.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

PROGRAM = test_server

include $(ROOT)/mak/program.mak
