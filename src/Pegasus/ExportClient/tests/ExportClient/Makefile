ROOT = ../../../../..
DIR = Pegasus/ExportClient/tests/ExportClient
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestExportClient
SOURCES = ExportClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
