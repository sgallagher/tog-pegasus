ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/ipinfo
include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcliutils \
    pegclient \
    peggetoopt \
    pegcommon

PROGRAM = ipinfo
SOURCES = IPInfoException.cpp \
          IPInfo.cpp \
          IPRoute.cpp \
          BIPTLEp.cpp \
          IPPEp.cpp \

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	$(PROGRAM)
