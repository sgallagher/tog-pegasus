ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/wbemexec

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    peggetoopt \
    pegclient \
    pegcliutils

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = wbemexec

SOURCES = \
        Channel.cpp \
        Handler.cpp \
        Selector.cpp \
        TCPChannel.cpp \
        WbemExecClientHandler.cpp \
        WbemExecClientHandlerFactory.cpp \
        WbemExecException.cpp \
        XMLProcess.cpp \
        WbemExecCommand.cpp

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
SYS_LIBS = ws2_32.lib advapi32.lib
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests:

install:
