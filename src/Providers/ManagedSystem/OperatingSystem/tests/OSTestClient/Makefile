ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/ManagedSystem/OperatingSystem/tests/OSTestClient

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegclient \
    pegcommon

PROGRAM = OSTestClient
SOURCES = OSTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
