include $(ROOT)/mak/config-unix.mak

OS = linux

ARCHITECTURE = iX86

COMPILER = gnu

SYS_INCLUDES = -I/usr/include/g++

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused -D_REENTRANT
else
FLAGS = -O2 -W -Wall -Wno-unused
endif

SYS_LIBS = -ldl -lpthread

CXX = g++

SH = sh

YACC = yacc

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

PEGASUS_HAS_MAKEDEPEND = yes
