ROOT=../../..

DIR=CommExample/Client

INCLUDES = -I..

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)comm$(LIB_SUFFIX) \
    $(ACE_LIB)

SOURCES = CommClient.cpp

PROGRAM = CommClient

include $(ROOT)/mak/program.mak
