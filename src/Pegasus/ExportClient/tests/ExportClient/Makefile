ROOT = ../../../../..
DIR = Pegasus/ExportClient/tests/ExportClient
include $(ROOT)/mak/config.mak
include ../libraries.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestExportClient

SOURCES = ExportClient.cpp

include $(ROOT)/mak/program.mak

run:
	$(PROGRAM)
