MAKE_TOOL = make

SYS_INCLUDES = 

OS_TYPE=unix

DEPEND_INCLUDES = -I/usr/include/g++-3

DEFINES = -DPEGASUS_OS_TYPE_UNIX

FLAGS = -O2 -W -Wall -pipe -Wno-unused -fcheck-new

RM = rm -f

SYS_LIBS = -ldl

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
