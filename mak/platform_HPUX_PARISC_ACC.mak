include $(ROOT)/mak/config-unix.mak

OS = hpux

ARCHITECTURE = parisc

COMPILER = acc

SYS_INCLUDES = -I$(ROOT)/src/stdcxx/stream

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

## Flags:
##     +Z - produces position independent code (PIC).
##

ifdef PEGASUS_DEBUG
FLAGS = -g +Z
else
FLAGS = +Z
endif

SYS_LIBS =

CXX = aCC

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .sl
