ROOT=../../../..
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES) $(ACE_INCLUDES)

LIBRARIES = $(ACE_LIB)

SOURCES = dlloader.cpp inst.cpp handler.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

PROGRAM = dlloader

include $(ROOT)/mak/program.mak
