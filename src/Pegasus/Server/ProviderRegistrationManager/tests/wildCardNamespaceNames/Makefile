ROOT = ../../../../../..
DIR = Pegasus/Server/ProviderRegistrationManager/tests/wildCardNamespaceNames
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = wildCardNamespaceNames

SOURCES = wildCardNamespaceNames.cpp

include $(ROOT)/mak/program.mak

tests:
	@$(PROGRAM)

poststarttests:
install:
