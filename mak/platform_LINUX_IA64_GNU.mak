include $(ROOT)/mak/config-unix.mak

OS = linux

ARCHITECTURE = IA64

COMPILER = gnu

SYS_INCLUDES = -I/usr/include/g++-3

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused  -fPIC -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
else
FLAGS = -O2 -W -Wall -Wno-unused -fPIC -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
endif

FLAGS += -DPEGASUS_PLATFORM_LINUX_GENERIC_GNU

#FLAGS += -DPEGASUS_HAS_SSL
#SYS_LIBS = -ldl -lpthread -lcrypt -lssl
SYS_LIBS = -ldl -lpthread -lcrypt


CXX = g++

SH = sh

YACC = yacc

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

