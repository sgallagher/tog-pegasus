ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/dnsinfo
include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = 

LIBRARIES = \
    pegcliutils \
    peggetoopt \
    pegclient \
    pegcommon

PROGRAM = dnsinfo
SOURCES = DNSInfo.cpp

include $(ROOT)/mak/program.mak
include $(ROOT)/mak/install.mak

tests:

poststarttests:
	$(PROGRAM)
