ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

LOCAL_DEFINES = -DUSE_CONNECTLOCAL

DIR = Clients/nisinfo
include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcliutils \
    peggetoopt \
    pegclient \
    pegcommon

PROGRAM = nisinfo
SOURCES = NISInfo.cpp 


include $(ROOT)/mak/program.mak
include $(ROOT)/mak/install.mak

tests:

poststarttests:
	$(PROGRAM)
