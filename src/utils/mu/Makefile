ifdef PEGASUS_OS_TYPE
    OS_TYPE=$(PEGASUS_OS_TYPE)
else
    OS_TYPE = windows
endif

ifeq ($(OS_TYPE),windows)
    OS_TYPE = windows
    CXX = cl
    CXXFLAGS = -nologo -W3 -GX
    DEFINES = -DOS_TYPE_WINDOWS
    PROGRAM = mu.exe
    EXE_OUT = -Fe
    RM = erase
    OBJ = .obj
else
    OS_TYPE = unix
    CXX = g++
    CXXFLAGS = -g -Wall -Wno-unused -pedantic
    DEFINES = -DOS_TYPE_UNIX
    PROGRAM = mu
    EXE_OUT = -o
    RM = rm -f
    OBJ = .o
endif

ifndef MU_BIN_DIR
MU_BIN_DIR = ./
endif

ifeq ($(OS_TYPE),windows)
    PLATFORM_SOURCES = FilesWindows.cpp
else
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
    TouchCmd.h

OBJECTS=$(SOURCES:.cpp=$(OBJ))

$(MU_BIN_DIR)$(PROGRAM): $(OBJECTS)
	$(CXX) $(EXE_OUT)$(MU_BIN_DIR)$(PROGRAM) $(OBJECTS)

ifeq ($(OS_TYPE),windows)
%.obj : %.cpp
else
%.o : %.cpp
endif
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS) $(DEFINES) $*.cpp

ifeq ($(OS_TYPE),windows)
    TMP_MU_BIN_DIR = $(subst /,\,$(MU_BIN_DIR))
else
    TMP_MU_BIN_DIR = $(MU_BIN_DIR)
endif

clean:
	- $(RM) $(TMP_MU_BIN_DIR)$(PROGRAM) 
	- $(RM) *$(OBJ)
