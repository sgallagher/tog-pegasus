OS_TYPE = windows

SYS_INCLUDES =

DEPEND_INCLUDES =

DEFINES =

ifdef PEGASUS_DEBUG
  FLAGS = -GX -W3 -Od -Zi -MDd
else
  FLAGS = -GX -W3 -O2 -MD
endif

RM = mu rm

RMDIRHIER = mu rmdirhier

MKDIRHIER = mu mkdirhier

COPY = mu copy

CXX = cl -nologo

EXE_OUT = -Fe

LIB_OUT = -out:

OBJ = .obj

OBJ_OUT = -Fo

EXE = .exe

AR = LINK -nologo -dll

LIB_PREFIX =

LIB_SUFFIX = .lib

SHARED_LIB_SUFFIX = .dll

COPY = mu copy

TOUCH = mu touch

ECHO = mu echo

LEX = flex

YACC = bison
