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
    LINK_FLAGS += /link /nodefaultlib:msvcrt
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

OBJECTS = $(SOURCES:.cpp=$(OBJ))

%$(OBJ): %.cpp
	$(CXX) -c $(OBJ_OUT)$@ $(FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp 


$(FULL_PROGRAM): $(OBJECTS)
	$(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXE_OUTPUT) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIB) $(LINK_FLAGS)
	$(MKDIRHIER) $(BIN_DIR)
	$(COPY) $(FULL_PROGRAM) $(BIN_DIR)/$(PROGRAM)$(EXE)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(FULL_PROGRAM)
	
tests:

poststarttests:
