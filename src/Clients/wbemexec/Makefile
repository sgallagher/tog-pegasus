ROOT = ../../..

DIR = Clients/wbemexec

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    peggetoopt \
    pegclient \
    pegcliutils

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = wbemexec

SOURCES = WbemExecException.cpp \
          WbemExecQueue.cpp \
          XMLProcess.cpp \
          WbemExecCommand.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests: 

install:
