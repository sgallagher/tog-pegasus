include $(ROOT)/mak/config-unix.mak

OS = tru64

ARCHITECTURE = alpha

COMPILER = deccxx

SYS_INCLUDES = -I$(ROOT)/src/stdcxx/cwrappers

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

ifdef PEGASUS_DEBUG
FLAGS = -gall
else
FLAGS = -O2
endif

#SYS_LIBS = -ldl
SYS_LIBS = -lrt

CXX = cxx -tlocal -pthread

SH = sh

YACC = bison

COPY = cp

MOVE = mv

PEGASUS_SUPPORTS_DYNLIB = yes

LIB_SUFFIX = .so
