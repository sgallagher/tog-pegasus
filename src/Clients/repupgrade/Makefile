ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/repupgrade

include $(ROOT)/mak/config.mak

LIBRARIES = \
    peggetoopt \
    pegrepository \
    pegcliutils \
    pegclient \
    pegconfig \
    pegcommon 

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = repupgrade

SOURCES = SSPModule.cpp \
          RepositoryUpgrade.cpp \
          RepositoryUpgradeException.cpp

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS = ws2_32.lib advapi32.lib
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests:
