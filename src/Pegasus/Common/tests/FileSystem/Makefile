ROOT = ../../../../..
DIR = Pegasus/Common/tests/FileSystem
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestFileSystem
SOURCES = FileSystem.cpp

include $(ROOT)/mak/program.mak

tests:
	$(PROGRAM)

poststarttests:
