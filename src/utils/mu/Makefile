ROOT= ../../..
DIR = utils/mu
include $(ROOT)/mak/config.mak

PROGRAM = mu

ifeq ($(OS_TYPE),windows)
    PLATFORM_SOURCES = FilesWindows.cpp
    DEFINES += -DOS_WINDOWS
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
    StripCmd.cpp \
    PrependCmd.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
