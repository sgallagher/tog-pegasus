ROOT = ../../../../..
DIR = Pegasus/ExportServer/tests/HandlerTable
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = HandlerTable

SOURCES = HandlerTable.cpp

include $(ROOT)/mak/program.mak

tests:
	HandlerTable

poststarttests:
