include $(ROOT)/mak/config-unix.mak

OS = solaris

ARCHITECTURE = sparc

COMPILER = gnu

SYS_INCLUDES = 

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D_POSIX_PTHREAD_SEMANTICS -D BSD_COMP -D_REENTRANT

DEFINES += -DDISABLE_KILLING_HUNG_THREADS

DEFINES += -DPEGASUS_OS_SOLARIS

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused -fPIC
DEFINES += -DPEGASUS_DEBUG
else
FLAGS = -O2 -W -Wall -Wno-unused -fPIC
endif

SYS_LIBS = -lpthread -ldl -lsocket -lnsl -lxnet -lrt

CXX = g++

SH = sh

YACC = yacc

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes
