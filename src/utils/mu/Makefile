################################################################################
#
# Makefile for mu.
#
# Building:
#
#     On Windows type "make OS=windows"
#     On Unix type "make"
#
################################################################################

OS=unix

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
OS=windows
endif

ifeq ($(PEGASUS_PLATFORM),TRU64_ALPHA_DECCXX)
OS=tru64
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
OS=hpux
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_IA64_ACC)
OS=hpux
endif

# Set Parameters for OS type
ifeq ($(OS),compaq_oss)
    CXX = c89
    CXXFLAGS = -Wversion2 -I /cw/rudys/nsm/pegasus/src
    LIBFLAGS = /usr/lib/crtlmain.o -obey /usr/lib/libc.obey
    SRLFLAGS = -l zrwslsrl -l zcplsrl -verbose
    LXX = nld
    DEFINES = -DOS_OSS
    PROGRAM = mu
    EXE_OUT = -o
    RM = rm -f
    OBJ = .o
    PLATFORM_SOURCES = FilesUnix.cpp
endif

ifeq ($(OS),windows)
    CXX = cl
    LXX = $(CXX)
    CXXFLAGS = -nologo -W3 -GX
    DEFINES = -DOS_WINDOWS
    PROGRAM = mu.exe
    EXE_OUT = -Fe
    RM = del
    OBJ = .obj
    PLATFORM_SOURCES = FilesWindows.cpp
endif

ifeq ($(OS),unix)
    CXX = g++
    LXX = $(CXX)
    CXXFLAGS = -O2 -Wall -Wno-unused -pedantic
    DEFINES = -DOS_UNIX
    PROGRAM = mu
    EXE_OUT = -o
    RM = rm -f
    OBJ = .o
    PLATFORM_SOURCES = FilesUnix.cpp
endif

ifeq ($(OS),tru64)
    CXX = g++
    LXX = $(CXX)
    CXXFLAGS = -O2 -Wall -Wno-unused -pedantic -I../../stdcxx/stream
    DEFINES = -DOS_TRU64
    PROGRAM = mu
    EXE_OUT = -o
    RM = rm -f
    OBJ = .o
    PLATFORM_SOURCES = FilesUnix.cpp
endif

ifeq ($(OS),hpux)
    FLAGS += +Z -D_POSIX_C_SOURCE=199506L -D_HPUX_SOURCE 
    ifeq ($(PEGASUS_PLATFORM),HPUX_IA64_ACC)
      FLAGS += -DPEGASUS_PLATFORM_HPUX_IA64_ACC -AP
      ifeq ($(HPUX_IA64_NATIVE_COMPILER),yes)
        FLAGS += -DHPUX_IA64_NATIVE_COMPILER
        INCLUDES = -I../../stdcxx/stream
      else
        INCLUDES = -I../../stdcxx/stream
      endif
    else
      FLAGS += -DPEGASUS_PLATFORM_HPUX_PARISC_ACC +DAportable
      INCLUDES = -I../../stdcxx/stream
    endif
    CXX = aCC
    LXX = $(CXX)
    CXXFLAGS = $(FLAGS) $(INCLUDES)
    DEFINES = -DOS_UNIX
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
	$(LXX) $(CXXFLAGS) $(LIBFLAGS) $(SRLFLAGS) $(EXE_OUT)$(PROGRAM) $(OBJECTS)

%$(OBJ) : %.cpp
	$(CXX) -c $(CXXFLAGS) $(DEFINES) $*.cpp

clean:
	- $(RM) $(PROGRAM)
	- $(RM) *$(OBJ)
