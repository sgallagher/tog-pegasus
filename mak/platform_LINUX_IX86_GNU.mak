include $(ROOT)/mak/config-unix.mak

OS = linux

ARCHITECTURE = iX86

COMPILER = gnu

SYS_INCLUDES = -I/usr/include/g++

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused  -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
else
FLAGS = -O2 -W -Wall -Wno-unused -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
endif

#FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_HAS_PERFINST

# ATTN KS 20020927 - Add flag to allow conditional testing of interoperability
# changes during interoperability tests.
ifdef PEGASUS_SNIA_INTEROP_TEST
 DEFINES+= -DPEGASUS_SNIA_INTEROP_TEST
endif

#SYS_LIBS = -ldl -lpthread -lcrypt -lssl
SYS_LIBS = -ldl -lpthread -lcrypt


CXX = g++

SH = sh

YACC = yacc

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

ifndef PEGASUS_USE_MU_DEPEND
PEGASUS_HAS_MAKEDEPEND = yes
endif
