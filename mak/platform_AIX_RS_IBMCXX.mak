include $(ROOT)/mak/config-unix.mak

OS = aix

ARCHITECTURE = rs

COMPILER = xlc

SYS_INCLUDES = -I/usr/vacpp/include

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

DEPEND_DEFINES = -D__IBMCPP__=500

ifdef PEGASUS_DEBUG
FLAGS = -g
else
FLAGS = -O2
endif

FLAGS += -qrtti=dyna 

SYS_LIBS = -ldl 

# SSL support
FLAGS += -DPEGAUS_HAS_SSL
SYS_INCLUDES += -I/usr/linux/include
SYS_LIBS += -L/usr/linux/lib -lssl

CXX = xlC_r

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

AIX_LIB_PRIORITY = 0

ifndef PEGASUS_USE_MU_DEPEND
PEGASUS_HAS_MAKEDEPEND = yes
endif

PEGASUS_SUPPORTS_DYNLIB = yes
