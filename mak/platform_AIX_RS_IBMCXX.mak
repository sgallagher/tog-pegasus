include $(ROOT)/mak/config-unix.mak

OS = aix

ARCHITECTURE = rs

COMPILER = xlc

SYS_INCLUDES = -I$(ROOT)/src/stdcxx

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

ifdef PEGASUS_DEBUG
FLAGS = -g
else
FLAGS = -O2
endif

SYS_LIBS = -ldl

CXX = xlC

SH = sh

YACC = bison

COPY = cp

MOVE = mv
