# Makefile for mu.
# Because mu is required for almost everything else, this makefile
# was built with minimal dependencies on other makes.
# Builds mu.exe in the local directory unless MU_BIN_DIR defined.
#
# Set default to windows if OS type not set.

ifdef PEGASUS_OS_TYPE
    OS_TYPE=$(PEGASUS_OS_TYPE)
else
    OS_TYPE = windows
endif

# Set Parameters for OS type
ifeq ($(OS_TYPE),compaq_oss)
    OS_TYPE = oss
    CXX = c89
    CXXFLAGS = -Wversion2 -I /cw/rudys/nsm/pegasus/src
    LIBFLAGS = /usr/lib/crtlmain.o -obey /usr/lib/libc.obey
    SRLFLAGS = -l zrwslsrl -l zcplsrl -verbose
    LXX = nld
    DEFINES = -DOS_TYPE_OSS
    PROGRAM = mu
    EXE_OUT = -o
    RM = rm -f
    OBJ = .o
    PLATFORM_SOURCES = FilesUnix.cpp
endif

ifeq ($(OS_TYPE),windows)
    OS_TYPE = windows
    CXX = cl
    LXX = $(CXX)
    CXXFLAGS = -nologo -W3 -GX
    DEFINES = -DOS_TYPE_WINDOWS
    PROGRAM = mu.exe
    EXE_OUT = -Fe
    RM = erase
    OBJ = .obj
    PLATFORM_SOURCES = FilesWindows.cpp
endif

ifeq ($(OS_TYPE),unix)
    OS_TYPE = unix
    CXX = g++
    LXX = $(CXX)
    CXXFLAGS = -g -Wall -Wno-unused -pedantic
    DEFINES = -DOS_TYPE_UNIX
    PROGRAM = mu
    EXE_OUT = -o
    RM = rm -f
    OBJ = .o
    PLATFORM_SOURCES = FilesUnix.cpp
endif

SOURCES = \
    $(PLATFORM_SOURCES) \
    DependCmd.cpp \
    PwdCmd.cpp \
    CompareCmd.cpp \
    MoveCmd.cpp \
    CopyCmd.cpp \
    TouchCmd.cpp \
    EchoCmd.cpp \
    MkDirHierCmd.cpp \
    RmDirHierCmd.cpp \
    RmCmd.cpp \
    Files.cpp \
    mu.cpp \
    StripCmd.cpp \
    PrependCmd.cpp 

HEADERS = \
    CompareCmd.h \
    Config.h \
    CopyCmd.h \
    DependCmd.h \
    EchoCmd.h \
    Files.h \
    MkDirHierCmd.h \
    MoveCmd.h \
    PwdCmd.h \
    RmCmd.h \
    RmDirHierCmd.h \
    TouchCmd.h \
    StripCmd.h \
    PrependCmd.h

OBJECTS=$(SOURCES:.cpp=$(OBJ))

$(PROGRAM): $(OBJECTS)
	$(LXX) $(LIBFLAGS) $(SRLFLAGS) $(EXE_OUT)$(PROGRAM) $(OBJECTS)

%$(OBJ) : %.cpp
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $*.cpp

clean:
	- $(RM) $(PROGRAM)
	- $(RM) *$(OBJ)
