ROOT = ../../../../../..
DIR = Pegasus/Server/ConfigurationManager/tests/modifyInstances
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = modifyRegistration

SOURCES = modifyInstances.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(PROGRAM)
