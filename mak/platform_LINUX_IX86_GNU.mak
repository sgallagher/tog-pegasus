include $(ROOT)/mak/config-unix.mak

OS = linux

ARCHITECTURE = iX86

COMPILER = gnu

SYS_INCLUDES = 

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES = -I/usr/include/g++-3

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused
else
FLAGS = -O2 -W -Wall -Wno-unused
endif

SYS_LIBS = -ldl

CXX = g++

SH = sh

YACC = yacc

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes
