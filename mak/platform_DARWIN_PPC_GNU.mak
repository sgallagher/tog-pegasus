include $(ROOT)/mak/config-unix.mak

OS = darwin

ARCHITECTURE = ppc

COMPILER = gnu

SYS_INCLUDES = 

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D_POSIX_PTHREAD_SEMANTICS -D BSD_COMP -D_REENTRANT

DEFINES += -DPEGASUS_OS_DARWIN

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused -fPIC
else
FLAGS = -O2 -W -Wall -Wno-unused -fPIC
endif

SYS_LIBS = -lpthread -ldl

CXX = g++

SH = sh

YACC = yacc

RM = rm -f

DIFF = diff

SORT = sort

COPY = cp

MOVE = mv

LIB_SUFFIX = .dylib

PEGASUS_SUPPORTS_DYNLIB = yes

ifdef PEGASUS_USE_RELEASE_DIRS
  PEGASUS_DEST_LIB_DIR=/usr/lib/pegasus
endif
