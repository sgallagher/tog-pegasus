ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Providers/ManagedSystem/OperatingSystem/tests/OSTestClient

include $(ROOT)/mak/config.mak
include $(ROOT)/src/Clients/libraries.mak

PROGRAM = OSTestClient
SOURCES = OSTestClient.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
# ATTN: This test has been commented out because it breaks on some systems
#ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
#	$(PROGRAM)
#endif
