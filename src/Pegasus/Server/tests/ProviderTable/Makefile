ROOT = ../../../../..
DIR = Pegasus/Server/tests/ProviderTable
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = ProviderTable

SOURCES = ProviderTable.cpp

include $(ROOT)/mak/program.mak

tests:
	ProviderTable
