include $(ROOT)/mak/config-unix.mak

OS = hpux

ARCHITECTURE = parisc

COMPILER = acc

SYS_INCLUDES = -I$(ROOT)/src/stdcxx/stream

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

## Flags:
##     +Z - produces position independent code (PIC).
##     +DAportable generates code for any HP9000 architecture
##     -Wl, passes the following option to the linker
##       +s causes the linked image or shared lib to be able to
##          search for any referenced shared libs dynamically in
##          SHLIB_PATH (LD_LIBRARY_PATH on 64-bit HP9000)
##       +b enables dynamic search in the specified directory(ies)
##

FLAGS = +Z +DAportable
ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
  FLAGS += -Wl,+b/usr/lib -Wl,+s
endif
ifdef PEGASUS_DEBUG
  FLAGS += -g
endif

SYS_LIBS =

CXX = aCC

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .sl
