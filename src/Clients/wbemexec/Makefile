ROOT = ../../..

DIR = Pegasus/Clients/wbemexec

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegprotocol \
    peggetoopt \
    pegclient \
    pegcliutils

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = wbemexec

SOURCES = WbemExecException.cpp \
          WbemExecClientHandler.cpp \
          WbemExecClientHandlerFactory.cpp \
          XMLProcess.cpp \
          WbemExecCommand.cpp

include $(ROOT)/mak/program.mak

tests:

install:
