MAKE_TOOL = make

SYS_INCLUDES =

OS_TYPE = windows

DEPEND_INCLUDES =

DEFINES = -DWIN32

FLAGS = -DPEGASUS_OS_TYPE_WINDOWS -GX -W3 -O2 -MD

# Debugging:
# FLAGS = -DPEGASUS_OS_TYPE_WINDOWS -GX -W3 -O2 -Zi -MDd

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

# A library called "xyz" in windows is of the form: xyz.lib

LIB_PREFIX =

LIB_SUFFIX = .lib

SHARED_LIB_SUFFIX = .dll

COPY = mu copy

TOUCH = mu touch

ECHO = mu echo

LEX = flex

YACC = bison
