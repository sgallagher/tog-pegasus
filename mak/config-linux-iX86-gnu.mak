include $(ROOT)/mak/config-unix.mak

OS = linux

ARCHITECTURE = iX86

COMPILER = gnu

SYS_INCLUDES = 

DEPEND_INCLUDES = -I/usr/include/g++-3

FLAGS = -O2 -W -Wall -Wno-unused

SYS_LIBS = -ldl

CXX = g++
