ROOT = ../../..

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/wbemexec

include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
  SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

LIBRARIES = \
    pegcommon \
    peggetoopt \
    pegclient \
    pegcliutils \
    pegconfig

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = wbemexec

SOURCES = \
        WbemExecClient.cpp \
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
