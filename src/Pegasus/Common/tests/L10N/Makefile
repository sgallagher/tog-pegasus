ROOT = ../../../../..
DIR = Pegasus/Common/tests/L10N

DIRS = msg 

include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestL10N
SOURCES = l10n.cpp

include $(ROOT)/mak/program.mak
include $(ROOT)/mak/recurse.mak

tests:
	$(PROGRAM)

