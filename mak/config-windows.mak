MAKE_TOOL = make

SYS_INCLUDES =

OS_TYPE = windows

DEPEND_INCLUDES =

ACE_INCLUDES = -I$(ACE_ROOT)

ifeq ($(PEGASUS_DEBUG),1)
ACE_LIB = $(ACE_ROOT)/ace/aced.lib
else
ACE_LIB = $(ACE_ROOT)/ace/ace.lib
endif

DEFINES = -DWIN32

FLAGS = -DPEGASUS_OS_TYPE_WINDOWS -GX -W3 -MDd -O2

RM = mu rm

RMDIRHIER = mu rmdirhier

MKDIRHIER = mu mkdirhier

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
