OS_TYPE = windows
OS = win32
ARCHITECTURE = iX86
COMPILER = msvc

SYS_INCLUDES =

DEPEND_INCLUDES =

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D_WIN32_WINNT=0x0400 -DPEGASUS_REMOVE_DEPRECATED
#-D_WIN32_WINNT=0x0400 -DWINVER=0x0400

ifdef PEGASUS_DEBUG
  FLAGS = -GX -GR -W3 -Od -Zi -MDd -Fd$(OBJ_DIR)
  LINK_FLAGS = -debug
else
  FLAGS = -GX -GR -W3 -O2 -MD
endif

ifdef PEGASUS_DEBUG_CIMEXCEPTION
 DEFINES += -DPEGASUS_DEBUG_CIMEXCEPTION
endif

# ATTN KS 20020927 - Add flag to allow conditional testing of interoperability
# changes during interoperability tests.
ifdef PEGASUS_SNIA_INTEROP_TEST
 DEFINES+= -DPEGASUS_SNIA_INTEROP_TEST
endif
 
RM = mu rm

RMDIRHIER = mu rmdirhier

MKDIRHIER = mu mkdirhier

COPY = mu copy

MOVE = mu move

CXX = cl -nologo

EXE_OUT = -Fe

LIB_OUT = -out:

OBJ = .obj

OBJ_OUT = -Fo

EXE = .exe

DLL = .dll

ILK = .ilk

PDB = .pdb

EXP = .exp

AR = LINK -nologo -dll

LIB_PREFIX =

LIB_SUFFIX = .lib

TOUCH = mu touch

ECHO = mu echo

LEX = flex

YACC = bison

SH = bash

YACC = bison
