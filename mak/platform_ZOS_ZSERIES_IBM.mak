include $(ROOT)/mak/config-unix.mak

OS = zos

ARCHITECTURE = zseries

COMPILER = ibm

SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include -I$(ROOT)/src/StandardIncludes/zOS

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

ifdef PEGASUS_DEBUG
FLAGS = -g -W c,dll,expo
PR_FLAGS = -g -W c,dll
else
FLAGS = -O2 -W c,dll,expo
PR_FLAGS = -O2 -W c,dll
endif

SYS_LIBS =

CXX = c++

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

PEGASUS_HAS_MAKEDEPEND = yes
