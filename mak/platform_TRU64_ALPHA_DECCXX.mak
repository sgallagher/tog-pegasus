include $(ROOT)/mak/config-unix.mak

OS = tru64

ARCHITECTURE = alpha

COMPILER = deccxx

SYS_INCLUDES = -I$(ROOT)/src/stdcxx/cwrappers

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

ifdef PEGASUS_DEBUG
FLAGS = -g
else
FLAGS = -O2
endif

#SYS_LIBS = -ldl

CXX = cxx

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .so
