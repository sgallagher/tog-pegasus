MAKE_TOOL = make

SYS_INCLUDES = 

OS_TYPE=unix

DEPEND_INCLUDES = -I/usr/include/g++-3

ACE_INCLUDES = -I$(ACE_ROOT)

ACE_LIB = $(ACE_ROOT)/ace/libACE.so

DEFINES =

FLAGS = -DPEGASUS_OS_TYPE_UNIX -O2 -W -Wall -pipe -Wno-unused -fcheck-new

RM = rm -f

RMDIRHIER = rm -rf

MKDIRHIER = mkdir -p 

CXX = g++

EXE_OUT = -o

OBJ = .o

OBJ_OUT = -o

EXE =

LIB_PREFIX = lib

LIB_SUFFIX = .so

# SHARED_LIB_SUFFIX = .so

COPY = cp

TOUCH = touch

ECHO = echo
