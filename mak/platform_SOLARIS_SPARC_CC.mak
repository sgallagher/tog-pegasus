include $(ROOT)/mak/config-unix.mak

OS = solaris

ARCHITECTURE = sparc

COMPILER = CC

SYS_INCLUDES = 

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D_POSIX_PTHREAD_SEMANTICS

DEFINES += -DDISABLE_KILLING_HUNG_THREADS

DEFINES += -DPEGASUS_OS_SOLARIS

DEFINES += -DPEGASUS_SNIA_INTEROP_TEST

# "READBUG" forces fstream.read to read 1 char at a time to
# overcome a bug in Wshop 6.2
# There are patches for this now.
#
# DEFINES += -DPEGASUS_OS_SOLARIS_READBUG

SUNOS_VERSION = $(shell uname -r)

ifeq ($(SUNOS_VERSION), 5.6)
DEFINES += -DSUNOS_5_6
endif

ifdef PEGASUS_DEBUG
FLAGS = -g -KPIC -mt -xs -xildoff
DEFINES += -DPEGASUS_DEBUG
else
FLAGS = -O -KPIC -mt -xildoff
endif

SYS_LIBS = -lpthread -ldl -lsocket -lnsl -lxnet

ifeq ($(SUNOS_VERSION), 5.6)
SYS_LIBS += -lposix4
else
SYS_LIBS += -lrt
endif

SYS_LIBS += $(EXTRA_LIBRARIES)

CXX = CC

SH = sh

YACC = yacc

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

SUNWS_CACHE = SunWS_cache

SOLARIS_COMPILER_PATH = /opt/SUNWspro/WS6U2
