ROOT = ../../../../../..
DIR = Pegasus/ControlProviders/ProviderRegistrationProvider/tests/createInstances
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = createRegistration

SOURCES = createInstances.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(PROGRAM)
