#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
DIR = utils/mu
include $(PEGASUS_ROOT)/mak/config.mak

PROGRAM = mu

OBJ_DIR = .
INCLUDES = -I$(PEGASUS_ROOT)/src $(EXTRA_INCLUDES)
EXE_OUTPUT = $(EXE_OUT)$(PROGRAM)$(EXE)

ifeq ($(OS_TYPE),windows)
    PLATFORM_SOURCES = FilesWindows.cpp
    DEFINES += -DOS_WINDOWS
    RM = del
    BACKWARD_SLASH = 1
else
    PLATFORM_SOURCES = FilesUnix.cpp
    OBJ_OUT = -o 
    RM = rm -f
endif

ifdef PEGASUS_MU_BIN_DIR
    ifdef BACKWARD_SLASH
       FULL_PROGRAM = $(PEGASUS_MU_BIN_DIR)\$(PROGRAM)$(EXE)
    else
       FULL_PROGRAM = $(PEGASUS_MU_BIN_DIR)/$(PROGRAM)$(EXE)
    endif
else
    FULL_PROGRAM = $(PROGRAM)$(EXE)
endif


SOURCES = \
    $(PLATFORM_SOURCES)     \
    DependCmd.cpp           \
    PwdCmd.cpp              \
    CompareCmd.cpp          \
    MoveCmd.cpp             \
    CopyCmd.cpp             \
    TouchCmd.cpp            \
    EchoCmd.cpp             \
    MkDirHierCmd.cpp        \
    RmDirHierCmd.cpp        \
    RmCmd.cpp               \
    Files.cpp               \
    mu.cpp                  \
    StripCmd.cpp            \
    PrependCmd.cpp          \
    SleepCmd.cpp            \
    SortCmd.cpp

OBJECTS = $(SOURCES:.cpp=$(OBJ))

%$(OBJ): %.cpp
	$(CXX) -c $(OBJ_OUT)$@ $(FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp 


$(FULL_PROGRAM): $(OBJECTS)
	$(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXE_OUTPUT) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIB)
	$(MKDIRHIER) $(BIN_DIR)
	$(COPY) $(FULL_PROGRAM) $(BIN_DIR)/$(PROGRAM)$(EXE)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(FULL_PROGRAM)

tests:

poststarttests:
